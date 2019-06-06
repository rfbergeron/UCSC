/*------------------------------------------------------------------------------
 * King.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Class representing king chesspiece and implementing attack function.
 *
 */

public class King extends Chesspiece {

    public King(int x, int y, Color c) {
        super(x, y, c);
    }

    boolean attack(Chesspiece target) {
        if(target != null && Math.abs(this.x - target.x) <= 1 && Math.abs(this.y - target.y) <= 1 && target.color != this.color) {
            return true;
        }
        else {
            return false;
        }
    }

    public String toString() {
        if(this.color == Color.WHITE) {
            return "k";
        }
        else {
            return "K";
        }
    }
}
