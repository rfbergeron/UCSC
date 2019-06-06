/*------------------------------------------------------------------------------
 * Node.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Node class for holding chesspieces on the board
 *
 */

public class Node {

    private Chesspiece item;
    private Node nextNode = null;

    public Node(Chesspiece piece) {
        this.item = piece;
    }

    public Chesspiece getItem() {
        return item;
    }

    public Node getNext() {
        return nextNode;
    }

    public void addNext(Chesspiece piece) {
        this.nextNode = new Node(piece);
    }
}
