/*------------------------------------------------------------------------------
 * Queen.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Represents queen piece, and implements attacks
 *
 */

public class Queen extends Chesspiece {

    public Queen(int x, int y, Color c) {
        super(x, y, c);
    }

    boolean attack(Chesspiece target) {

        if(target.color != this.color) {
            return this.x == target.x ||
                    this.y == target.y ||
                    Math.abs(this.x - target.x) == Math.abs(this.y - target.y);
        }
        else {
            return false;
        }

    }

    public String toString() {
        if(this.color == Color.WHITE) {
            return "q";
        }
        else {
            return "Q";
        }
    }
}
