// John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)
// $Id: cixd.cpp,v 1.7 2016-05-09 16:01:56-07 - - $

#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "logstream.h"
#include "protocol.h"
#include "sockets.h"

logstream logger (cout);
struct cix_exit: public exception {};

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
            cerr << logger.execname () << ": "
                 << "-" << char (optopt) << ": invalid option" << endl;
            break;
      }
   }
}

void reply_ls (accepted_socket& client_sock, cix_header& header) {
   const char* ls_cmd = "ls -l 2>&1";
   FILE* ls_pipe = popen (ls_cmd, "r");

   if (ls_pipe == NULL) {
      cerr << "ls -l: popen failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
      return;
   }

   string ls_output;
   char buffer[0x1000];
   for (;;) {
      char* rc = fgets (buffer, sizeof buffer, ls_pipe);
      if (rc == nullptr) break;
      ls_output.append (buffer);
   }
   int status = pclose (ls_pipe);
   if (status < 0) {
      DEBUGF ('y', ls_cmd << ": " << strerror (errno));
   } else {
      DEBUGF ('y',
              ls_cmd << ": exit " << (status >> 8) << " signal "
                     << (status & 0x7F) << " core "
                     << (status >> 7 & 1));
   }

   header.command = cix_command::LSOUT;
   header.nbytes = ls_output.size ();
   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('y', "sending header " << header);
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, ls_output.c_str (), ls_output.size ());
   DEBUGF ('y', "sent " << ls_output.size () << " bytes");
}

void reply_rm (accepted_socket& client_sock, cix_header& header) {
   // Call the remove command with the file
   int status;
   status = unlink (header.filename);

   if (status < 0) {
      cerr << "rm " << header.filename
           << ": unlink failed: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
      return;
   }
   // If the command returns, command failed
   header.command = cix_command::ACK;
   header.nbytes = 0;
   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('y', "sending packet: " << header);
   send_packet (client_sock, &header, sizeof header);
   return;
}

void reply_put (accepted_socket& client_sock, cix_header& header) {
   ofstream outstream ({header.filename}, ios_base::binary);
   // receive header, filename, nbytes in first packet
   if ((outstream.rdstate () & ifstream::failbit) != 0) {
      // If the pipe doesn't work, send NAK header back to client
      cerr << "put " << header.filename
           << ": failed to write file: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
      return;
   }

   if (header.nbytes > 0) {
      // char* buffer = new char[header.nbytes + 1];
      auto buffer = make_unique<char[]> (header.nbytes + 1);
      // receive the second packet with file contents, size of file
      recv_packet (client_sock, buffer.get (), header.nbytes);
      DEBUGF ('y', "received " << header.nbytes << " bytes");
      buffer[header.nbytes] = '\0';
      outstream.write (buffer.get (), header.nbytes);
   } else {
      DEBUGF ('y', "received 0 bytes");
   }

   // close the file after
   outstream.close ();
   // return ACK header back to client
   header.command = cix_command::ACK;
   header.nbytes = 0;
   memset (header.filename, 0, FILENAME_SIZE);
   DEBUGF ('y', "sending header " << header);
   send_packet (client_sock, &header, sizeof header);
}

void reply_get (accepted_socket& client_sock, cix_header& header) {
   ifstream instream ({header.filename}, ios_base::binary);

   if ((instream.rdstate () & ifstream::failbit) != 0) {
      cerr << "get " << header.filename
           << ": failed to read file: " << strerror (errno) << endl;
      header.command = cix_command::NAK;
      header.nbytes = errno;
      send_packet (client_sock, &header, sizeof header);
      return;
   }

   string stream_data;
   auto buffer = make_unique<char[]> (0x1000);
   // char * buffer = new char[0x1000];
   while (instream.read (buffer.get (), sizeof buffer)) {
      stream_data.append (buffer.get ());
   }

   instream.close ();
   header.command = cix_command::FILEOUT;
   header.nbytes = stream_data.size ();
   DEBUGF ('y', "sending header " << header);
   send_packet (client_sock, &header, sizeof header);
   send_packet (client_sock, stream_data.c_str (), stream_data.size ());
   DEBUGF ('y', "sent " << stream_data.size () << " bytes");
   // delete[] buffer;
}

void run_server (accepted_socket& client_sock) {
   logger.execname (logger.execname () + "-server");
   DEBUGF ('y', "connected to " << to_string (client_sock));
   try {
      for (;;) {
         cix_header header;
         recv_packet (client_sock, &header, sizeof header);
         DEBUGF ('y', "received header " << header);
         switch (header.command) {
            case cix_command::LS:
               reply_ls (client_sock, header);
               break;
            case cix_command::GET:
               reply_get (client_sock, header);
               break;
            case cix_command::PUT:
               reply_put (client_sock, header);
               break;
            case cix_command::RM:
               reply_rm (client_sock, header);
               break;
            default:
               cerr << "invalid header from client:" << header << endl;
               break;
         }
      }
   } catch (socket_error& error) {
      cerr << error.what () << endl;
   } catch (cix_exit& error) {
      cerr << "caught cix_exit" << endl;
   }
   DEBUGF ('y', "finishing");
   throw cix_exit ();
}

void fork_cixserver (server_socket& server, accepted_socket& accept) {
   pid_t pid = fork ();
   if (pid == 0) {   // child
      server.close ();
      run_server (accept);
      throw cix_exit ();
   } else {
      accept.close ();
      if (pid < 0) {
         cerr << "fork failed: " << strerror (errno) << endl;
      } else {
         DEBUGF ('y', "forked cixserver pid " << pid);
      }
   }
}

void reap_zombies () {
   for (;;) {
      int status;
      pid_t child = waitpid (-1, &status, WNOHANG);
      if (child <= 0) break;
      DEBUGF ('y',
              "child " << child << " exit " << (status >> 8)
                       << " signal " << (status & 0x7F) << " core "
                       << (status >> 7 & 1));
   }
}

void signal_handler (int signal) {
   DEBUGF ('y', "signal_handler: caught " << strsignal (signal));
   reap_zombies ();
}

void signal_action (int signal, void (*handler) (int)) {
   struct sigaction action;
   action.sa_handler = handler;
   sigfillset (&action.sa_mask);
   action.sa_flags = 0;
   int rc = sigaction (signal, &action, nullptr);
   if (rc < 0)
      cerr << "sigaction " << strsignal (signal)
           << " failed: " << strerror (errno) << endl;
}

void usage () {
   cerr << "Usage: " << logger.execname () << " [-@ flag] [port]"
        << endl;
   throw cix_exit ();
}

int main (int argc, char** argv) {
   logger.execname (basename (argv[0]));
   if (argc > 4) usage ();
   scan_options (argc, argv);
   DEBUGF ('y', "starting");
   unsigned int args_start = 1;
   if (argc > 1 && argv[1][0] == '-') args_start = 3;
   vector<string> args (&argv[args_start], &argv[argc]);
   signal_action (SIGCHLD, signal_handler);
   in_port_t port = get_cix_server_port (args, 0);
   try {
      server_socket listener (port);
      for (;;) {
         DEBUGF ('y',
                 to_string (hostinfo ())
                       << " accepting port " << to_string (port));
         accepted_socket client_sock;
         for (;;) {
            try {
               listener.accept (client_sock);
               break;
            } catch (socket_sys_error& error) {
               switch (error.sys_errno) {
                  case EINTR:
                     DEBUGF ('y',
                             "listener.accept caught "
                                   << strerror (EINTR));
                     break;
                  default:
                     throw;
               }
            }
         }
         DEBUGF ('y', "accepted " << to_string (client_sock));
         try {
            fork_cixserver (listener, client_sock);
            reap_zombies ();
         } catch (socket_error& error) {
            DEBUGF ('y', error.what ());
         }
      }
   } catch (socket_error& error) {
      cerr << error.what () << endl;
   } catch (cix_exit& error) {
      cerr << "caught cix_exit" << endl;
   }
   DEBUGF ('y', "finishing");
   return 0;
}
