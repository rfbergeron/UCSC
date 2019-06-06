/*------------------------------------------------------------------------------
 * Rook.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Class representing rook pieces. Implements attack function.
 *
 */

public class Rook extends Chesspiece {

    public Rook(int x, int y, Color c) {
        super(x, y, c);
    }

    boolean attack(Chesspiece target) {
        return target != null && this.x == target.x || this.y == target.y && target.color != this.color;
    }

    public String toString() {
        if(this.color == Color.WHITE) {
            return "r";
        }
        else {
            return "R";
        }
    }
}
