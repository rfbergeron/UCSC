import java.io.*;
import java.util.Scanner;

/*------------------------------------------------------------------------------
 * Author: Robert Bergeron
 * ID: rbergero
 * Reads input from file specified in first argument and writes each word,
 * backwards, on a separate line in another file, specified in second argument
 */


public class FileReverse {

    public static void main(String[] args) throws IOException{
        // check number of command line arguments is at least 2
        if(args.length < 2){
            System.out.println("Usage: java –jar FileReverse.jar <input file> <output file>");
                    System.exit(1);
        }

        Scanner in = new Scanner(new File(args[0]));
        PrintWriter out = new PrintWriter(new FileWriter(args[1]));

        while(in.hasNextLine()) {
            String line = in.nextLine().trim() + " ";
            String[] tokens = line.split("\\s+");

            for(String token : tokens) {
                out.println(stringReverse(token));
            }

        }

        in.close();
        out.close();
    }

    public static String stringReverse(String s) {
        
        if(s.length() <= 0) {
            return "";
        }
        else {
            return stringReverse(s.substring(1)) + s.substring(0, 1);
        }
    }
}
