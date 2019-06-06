import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

/*------------------------------------------------------------------------------
 * Chessboard.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Main class for chessboard jar; handles most logic for finding pieces and
 * attacking
 */

public class Chessboard {

    private Node head;
    private int size;

    public Chessboard(int size) {
        this.size = size;
        this.head = new Node(null);

    }

    public static void main(String[] args) throws IOException {
        // check number of command line arguments is at least 2
        if (args.length < 2) {
            System.out.println("Usage: java –jar ChessBoard.jar <input file> <output file>");
            System.exit(1);
        }

        Scanner in = new Scanner(new File(args[0]));
        PrintWriter out = new PrintWriter(new FileWriter(args[1]));

        while(in.hasNextLine()) {
            Chessboard board = new Chessboard(8);
            String input = in.nextLine();
            int breakpoint = input.indexOf(':');
            String[] attackerPos = input.substring(0, breakpoint).trim().split("\\s");
            String[] pieces = input.substring(breakpoint + 1).trim().split("\\s");

            board.placePieces(pieces);

            if(board.validate()) {
                Chesspiece attacker = board.find(Integer.parseInt(attackerPos[0]), Integer.parseInt(attackerPos[1]));

                if(attacker != null) {
                    out.print(attacker.toString() + " ");

                    if(board.canAttack(attacker) != null) {
                        out.println("y");
                    }
                    else {
                        out.println("n");
                    }
                }
                else {
                    out.println("-");
                }
            }
            else {
                out.println("Invalid");
            }
        }

        in.close();
        out.close();
    }

    public Chesspiece find(int x, int y) {
        Node current = this.head.getNext();

        while(current != null) {
            Chesspiece piece = current.getItem();

            if(piece.x == x && piece.y == y) {
                return piece;
            }
            else {
                current = current.getNext();
            }
        }

        return null;
    }

    public Chesspiece canAttack(Chesspiece attacker) {
        Node current = this.head.getNext();

        while(current != null) {
            Chesspiece defender = current.getItem();

            if(!attacker.equals(defender) && attacker.attack(defender)) {
                return defender;
            }
            else {
                current = current.getNext();
            }
        }

        return null;
    }

    public boolean validate() {
        Node current = this.head.getNext();
        boolean wKing = false;
        boolean bKing = false;

        while(current != null) {
            Node toCompare = current.getNext();
            while (toCompare != null) {
                Chesspiece piece1 = current.getItem();
                Chesspiece piece2 = toCompare.getItem();

                //System.out.println(piece1.toString() + " " + piece1.x + " " + piece1.y);

                /*
                if (piece1.toString().equals("k")) {
                    if (!wKing) {
                        wKing = true;
                    } else {
                        System.out.println("TWO WHITES");
                        return false;
                    }
                } else if(piece1.toString().equals("K")){
                    if (!bKing) {
                        bKing = true;
                    } else {
                        System.out.println("TWO BLACKS");
                        return false;
                    }
                }*/

                if(piece2.toString().equals("k") || piece1.toString().equals("k")) {
                    wKing = true;
                    if(piece2.toString().equals(piece1.toString())) {
                        return false;
                    }
                }
                else if(piece2.toString().equals("K") || piece1.toString().equals("K")) {
                    bKing = true;
                    if(piece2.toString().equals(piece1.toString())) {
                        return false;
                    }
                }

                if (piece1.x == piece2.x && piece1.y == piece2.y) {
                    return false;
                }

                toCompare = toCompare.getNext();
            }

            current = current.getNext();
        }

        return wKing && bKing;
    }

    public void addPiece(Chesspiece toAdd) {
        Node current = this.head;

        while(current.getNext() != null) {
            current = current.getNext();
        }

        current.addNext(toAdd);
    }

    public void placePieces(String[] data) {

        for(int i = 0 ; i < data.length ; i += 3) {
            switch(data[i]) {
                case "R":
                    addPiece(new Rook(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.BLACK));
                    break;
                case "K":
                    addPiece(new King(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.BLACK));
                    break;
                case "B":
                    addPiece(new Bishop(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.BLACK));
                    break;
                case "Q":
                    addPiece(new Queen(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.BLACK));
                    break;
                case "r":
                    addPiece(new Rook(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.WHITE));
                    break;
                case "k":
                    addPiece(new King(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.WHITE));
                    break;
                case "b":
                    addPiece(new Bishop(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.WHITE));
                    break;
                case "q":
                    addPiece(new Queen(Integer.parseInt(data[i + 1]), Integer.parseInt(data[i + 2]), Color.WHITE));
                    break;

            }
        }
    }
}
