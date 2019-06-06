import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.Stack;

//----------------------------------------------------------------------------------------------
//  Bard
//  Author: Robert Bergeron
//  ID: rbergero
//  Uses a stack of strings to examine individual words from Shakespeare file. Main file.
//----------------------------------------------------------------------------------------------

public class Bard {

    public static void main(String[] args) throws IOException {

        /*
        if (args.length < 0) {
            System.out.println("Usage: java –jar Bard.jar <input file>");
            System.exit(1);
        }*/
	String dir = System.getProperty("user.dir");
        Scanner in = new Scanner(new File(dir + "/input.txt"));
        PrintWriter out = new PrintWriter(new FileWriter(dir + "/analysis.txt"));
        Bard bard = new Bard();

        while(in.hasNextLine()) {
            Stack<String> shakespeare = bard.readShakespeare();
            String currentInput = in.nextLine();
            try{
                String[] tokens = currentInput.split(" ");
                int length = Integer.parseInt(tokens[0]);
                int maxOut = Integer.parseInt(tokens[1]);
                ArrayList<Stack<String>> outs = new ArrayList<>();

                while(!shakespeare.isEmpty()) {
                    String current = shakespeare.pop();

                    if(current.length() == length) {
                        nextString:
                        if(!outs.isEmpty()) {
                            for(Stack<String> currOut : outs) {
                                if(currOut.contains(current)) {
                                    currOut.push(current);
                                    break nextString;
                                }
                            }
                            outs.add(new Stack<>());
                            outs.get(outs.size() - 1).push(current);
                        }
                        else {
                            outs.add(new Stack<>());
                            outs.get(0).push(current);
                        }
                    }
                }
                int count = 0;

                while(!outs.isEmpty() && count < maxOut) {
                    int largestIndex = 0;

                    for(int i = 1 ; i < outs.size() ; i++) {
                        if(outs.get(i).size() > outs.get(largestIndex).size()) {
                            largestIndex = i;
                        }
                    }

                    out.print(outs.get(largestIndex).peek() + " ");
                    outs.remove(largestIndex);
                    count++;
                }
                out.print("\n");
            }
            catch(NumberFormatException e) {
                int count = 0;
                while(!shakespeare.isEmpty()) {
                    String current = shakespeare.pop();

                    if(current.equals(currentInput)) {
                        count++;
                    }
                }

                out.print(currentInput + " " + count + "\n");
            }
        }

        /*while(!shakespeare.isEmpty()) {
            String current = shakespeare.pop();
            out.print(current + "\n");
        }*/
        in.close();
        out.close();
    }

    public Stack<String> readShakespeare() throws IOException{
        Scanner shake = new Scanner(new File(System.getProperty("user.dir") + "/Shakespeare.txt"));
        Stack<String> words = new Stack<>();
        String current;

        while(shake.hasNextLine()) {
            current = shake.nextLine();
            current = current.replaceAll("[^\\p{L} ]", " ");
            String[] tokens = current.split(" ");

            for(String token : tokens) {
                token = token.trim();
                if(!token.isEmpty())
                    words.push(token);
            }
        }

        return words;
    }
}
