import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

//  Stack
//  Author: Robert Bergeron
//  ID: rbergero
//  Stack which is used to keep track of opening and closing braces in a String. Main file.
//----------------------------------------------------------------------------------------------

public class Stack {
    private Entry top;

    public void push(char item) {
        if(!this.isEmpty()) {
            Entry newTop = new Entry(item, this.top.getPosition() + 1, this.top);
            this.top = newTop;
        }
        else {
            this.top = new Entry(item, 0);
        }
    }

    public Entry pop() {
        if(!this.isEmpty()) {
            Entry toRemove = this.top;
            this.top = top.getNext();
            return toRemove;
        }
        else {
            return null;
        }
    }

    public Entry peek() {
        return this.top;
    }

    public boolean isEmpty() {
        return this.top == null;
    }

    public static void main(String[] args) throws IOException{

        // check number of command line arguments is at least 2
        if (args.length < 2) {
            System.out.println("Usage: java –jar ChessBoard.jar <input file> <output file>");
            System.exit(1);
        }

        Scanner in = new Scanner(new File(args[0]));
        PrintWriter out = new PrintWriter(new FileWriter(args[1]));
        int numlines = 0;


        while(in.hasNextLine()) {
            char[] currentLine = in.nextLine().toCharArray();
            boolean markedInvalid = false;
            Stack stack = new Stack();
            numlines++;

            nextCycle:
            for(char currentChar : currentLine) {
                switch(currentChar) {
                    case '{':
                    case '[':
                    case '(':
                    case '<':
                        stack.push(currentChar);
                        break;
                    case '}':
                        if(!stack.isEmpty() && stack.peek().getItem() == '{') {
                            stack.pop();
                            break;
                        }
                        else {
                            out.println("N");
                            markedInvalid = true;
                            break nextCycle;
                        }
                    case ']':
                        if(!stack.isEmpty() && stack.peek().getItem() == '[') {
                            stack.pop();
                            break;
                        }
                        else {
                            out.println("N");
                            markedInvalid = true;
                            break nextCycle;
                        }
                    case ')':
                        if(!stack.isEmpty() && stack.peek().getItem() == '(') {
                            stack.pop();
                            break;
                        }
                        else {
                            out.println("N");
                            markedInvalid = true;
                            break nextCycle;
                        }
                    case '>':
                        if(!stack.isEmpty() && stack.peek().getItem() == '<') {
                            stack.pop();
                            break;
                        }
                        else {
                            out.println("N");
                            markedInvalid = true;
                            break nextCycle;
                        }
                }
            }

            if(stack.isEmpty() && !markedInvalid) {
                out.println("Y");
            }
            else if(!markedInvalid){
                out.println("N");
            }
        }

        in.close();
        out.close();
    }
}
