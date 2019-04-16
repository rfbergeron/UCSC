// John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)
// $Id: cix.cpp,v 1.7 2019-02-07 15:14:37-08 - - $

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"
#include "debug.h"

logstream logger (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get", cix_command::GET  },
   {"put", cix_command::PUT  },
   {"rm", cix_command::RM    },
};

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            cerr << logger.execname() << ": " <<
               "-" << char (optopt) << ": invalid option";
            break;
      }
   }
}

static const string help = R"||(
exit         - Exit the program.  Equivalent to EOF.
get filename - Copy remote file to local host.
help         - Print help summary.
ls           - List names of files on remote server.
put filename - Copy local file to remote host.
rm filename  - Remove file from remote server.
)||";

void cix_help() {
   cout << help;
}

void cix_get (client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::GET;
   snprintf(header.filename, 
      sizeof (header.filename), filename.data());
   DEBUGF('y', "sending header " << header);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF('y', "received header " << header);
   if (header.command != cix_command::FILEOUT) {
      cerr << "sent GET, server did not return FILEOUT" << endl;
      cerr << "server returned " << header << endl;
   }else {
      ofstream outstream({header.filename}, ios_base::binary);
      if((outstream.rdstate() & ifstream::failbit) != 0) {
         cerr << "get " << header.filename <<
            ": failed to write file" << endl;
         return;
      }

      //char * buffer = new char[header.nbytes + 1];
      if(header.nbytes > 0) {
         auto buffer = make_unique<char[]> (header.nbytes + 1);
         recv_packet (server, buffer.get(), header.nbytes);
         DEBUGF('y', "received " << header.nbytes << " bytes");
         buffer[header.nbytes] = '\0';
         outstream.write(buffer.get(), header.nbytes);
      }
      else {
         DEBUGF('y', "received 0 bytes");
      }

      outstream.close();
   }
}

void cix_rm (client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::RM;
   // Copy name of file to header
   snprintf(header.filename,
      sizeof header.filename, filename.data());
   DEBUGF('y', "sending header " << header);
   send_packet (server, &header, sizeof header);
   // Receive packet back from server
   recv_packet (server, &header, sizeof header);
   DEBUGF('y', "received header " << header);
   if (header.command != cix_command::ACK) {
      cerr << "sent RM, server did not return ACK" << endl;
      cerr << "server returned " << header << endl;
   }
}

void cix_put(client_socket& server, string filename) {
   cix_header header;
   ifstream instream(filename, ios_base::binary);

   // File is open or if file can't open, return error
   if((instream.rdstate() & ifstream::failbit) != 0) {
      cerr << "failed to open file " << header.filename << endl;
      return;
   }
   header.command = cix_command::PUT;
   // Copy name of file to header
   snprintf(header.filename,
      sizeof header.filename, filename.data());

   string stream_data;
   //char * buffer = new char[0x1000];
   auto buffer = make_unique<char[]> (0x100);
   while(instream.read(buffer.get(), sizeof buffer)) {
      stream_data.append(buffer.get());
   }

   instream.close();
   header.nbytes = stream_data.size();
   DEBUGF('y', "sending header " << header);
   send_packet (server, &header, sizeof header);
   send_packet (server, stream_data.c_str(), stream_data.size());
   DEBUGF('y', "sent " << stream_data.size() << " bytes");
   recv_packet (server, &header, sizeof header);

   if (header.command != cix_command::ACK) {
      cerr << "sent PUT, server did not return ACK" << endl;
      cerr << "server returned " << header << endl;
   } else {
      DEBUGF('y', "received header " << header);
   }
   //delete[] buffer;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   DEBUGF('y', "sending header " << header);
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   DEBUGF('y', "received header " << header);
   if (header.command != cix_command::LSOUT) {
       cerr << "sent LS, server did not return LSOUT"  << endl;
       cerr << "server returned " << header << endl;
   }
   else {
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      recv_packet (server, buffer.get(), header.nbytes);
      DEBUGF('y', "received " << header.nbytes << " bytes");
      buffer[header.nbytes] = '\0';
      cout << buffer.get();
   }
}

void usage() {
   cerr << "Usage: " << logger.execname() <<
      " [-@ flag] [host] [port]" << endl;
   throw cix_exit();
}

vector<string> split (const string& line, const string& delimiters) {
   vector<string> words;
   size_t end = 0;

   // Loop over the string, splitting out words, and for each word
   // thus found, append it to the output wordvec.
   for (;;) {
      size_t start = line.find_first_not_of (delimiters, end);
      if (start == string::npos) break;
      end = line.find_first_of (delimiters, start);
      words.push_back (line.substr (start, end - start));
   }
   return words;
}

int main (int argc, char** argv) {
   logger.execname (basename (argv[0]));
   if (argc > 5) usage();
   scan_options(argc, argv);
   DEBUGF('y', "starting");
   unsigned int args_start = 1;
   if(argc > 1 && argv[1][0] == '-') args_start = 3;
   vector<string> args (&argv[args_start], &argv[argc]);
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   DEBUGF('y', to_string (hostinfo()));
   try {
      DEBUGF('y', "connecting to " << host << " port " << port);
      client_socket server (host, port);
      DEBUGF('y', "connected to " << to_string (server));
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         DEBUGF('y', "command " << line);

         vector<string> words = split(line, " ");

         const auto& itor = command_map.find (words.at(0));
         cix_command cmd = itor == command_map.end()
                            ? cix_command::ERROR : itor->second;

         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
               if(words.size() > 1) {
                  if(words.at(1).length() > 58) {
                     cerr << "no filenames over 58 characters" << endl;
                  }
                  else {
                     cix_get(server, words.at(1));
                  }
               }
               else {
                  cerr << "no filename provided for get" << endl;
               }
               break;
            case cix_command::PUT:
               if(words.size() > 1) {
                  if(words.at(1).length() > 58) {
                     cerr << "no filenames over 58 characters" << endl;
                  }
                  else {
                     cix_put(server, words.at(1));
                  }
               }
               else {
                  cerr << "no filename provided for put" << endl;
               }
               break;
            case cix_command::RM:
               if(words.size() > 1) {
                  if(words.at(1).length() > 58) {
                     cerr << "no filenames over 58 characters" << endl;
                  }
                  else {
                     cix_rm(server, words.at(1));
                  }
               }
               else {
                  cerr << "no filename provided for rm" << endl;
               }
               break;
            default:
               cerr << words.at(0) << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      cerr << error.what() << endl;
   }catch (cix_exit& error) {
      cerr << "caught cix_exit" << endl;
   }
   DEBUGF('y', "finishing");
   return 0;
}
