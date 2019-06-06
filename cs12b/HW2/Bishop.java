/*------------------------------------------------------------------------------
 * Bishop.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Class representing the bishop chess piece. Implements attack function.
 *
 */

public class Bishop extends Chesspiece {

    public Bishop(int x, int y, Color c) {
        super(x, y, c);
    }

    boolean attack(Chesspiece target) {
        return target != null && Math.abs(this.x - target.x) == Math.abs(this.y - target.y) && target.color != this.color;
    }

    public String toString() {
        if(this.color == Color.WHITE) {
            return "b";
        }
        else {
            return "B";
        }
    }
}
