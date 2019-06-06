// RefQueue
// Author: Robert Bergeron
// ID: rbergero
// Code for reference-based queue; also main class for HW4

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

public class RefQueue {

    private Node lastNode = null;

    public boolean isEmpty() {
            return lastNode == null;
    }

    public void enqueue(Object newItem) {
        if(isEmpty()) {
            lastNode = new Node(newItem);
        }
        else {
            lastNode.addNext(newItem);
        }
    }

    public Object dequeue() {
        Object temp = this.lastNode.getItem();
        lastNode = lastNode.getNext();
        return temp;
    }

    public void dequeueAll() {
        while(!isEmpty()) {
            this.dequeue();
        }
    }

    public Object peek() {
        return this.lastNode.getItem();
    }

    public static void main(String[] args) throws IOException{
        String input;
        RefQueue[] tellers = new RefQueue[5];

        for(int i = 0 ; i < tellers.length ; i++) {
             tellers[i] = new RefQueue();
        }

        // check number of command line arguments is at least 2
        if (args.length < 2) {
            System.out.println("Usage: java –jar SlugFest.jar <input file> <output file>");
            System.exit(1);
        }

        Scanner in = new Scanner(new File(args[0]));
        PrintWriter out = new PrintWriter(new FileWriter(args[1]));
        int[] lastTimes = {0, 0, 0, 0, 0};

        while(in.hasNextLine()) {
            input = in.nextLine().trim() + " ";
            String[] tokens = input.split(" ");
            int line = Integer.parseInt(tokens[0]) - 1;
            String name = tokens[1];
            int time = Integer.parseInt(tokens[2]);
            int transaction = Integer.parseInt(tokens[3]);

            if(tellers[line].isEmpty()) {
                tellers[line].enqueue(new QueueEvent(name, " begins ", time));
                tellers[line].enqueue(new QueueEvent(name, " ends ", time + transaction));
                lastTimes[line] = (time + transaction);
                //System.out.println(line + " " + lastTimes[line]);
            }
            else {

                if(lastTimes[line] > time) {
                    //System.out.println(line + " " + lastTimes[line]);
                    tellers[line].enqueue(new QueueEvent(name, " begins ", lastTimes[line]));
                    tellers[line].enqueue(new QueueEvent(name, " ends ", lastTimes[line] + transaction));
                    lastTimes[line] += transaction;
                    //System.out.println(line + " " + lastTimes[line]);
                }
                else {
                    //System.out.println(line + " " + time);
                    tellers[line].enqueue(new QueueEvent(name, " begins ", time));
                    tellers[line].enqueue(new QueueEvent(name, " ends ", time + transaction));
                    lastTimes[line] = time + transaction;
                    //System.out.println(line + " " + lastTimes[line]);
                }
            }
        }


        while(!tellers[0].isEmpty() ||
                !tellers[1].isEmpty() ||
                !tellers[2].isEmpty() ||
                !tellers[3].isEmpty() ||
                !tellers[4].isEmpty()) {
            int nextTime = Integer.MAX_VALUE;
            int number = -1;

            for(int i = 0 ; i < tellers.length ; i++) {
                RefQueue teller = tellers[i];

                if(!teller.isEmpty()) {
                    QueueEvent current = (QueueEvent) teller.peek();
                    if(current.time < nextTime) {
                        nextTime = current.time;
                        number = i;
                    }
                }
            }

            out.println((number+ 1) + " " + tellers[number].dequeue().toString());
        }

        in.close();
        out.close();
    }
}
