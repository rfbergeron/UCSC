/*------------------------------------------------------------------------------
 * Chesspiece.java
 * Author: Robert Bergeron
 * ID: rbergero
 * Abstract class that contains basic characteristics of a chesspiece
 *
 */

public abstract class Chesspiece {
    int x;
    int y;
    Color color;

    public Chesspiece(int x, int y, Color c) {
        this.x = x;
        this.y = y;
        this.color = c;
    }

    abstract boolean attack(Chesspiece target);

    @Override
    public boolean equals(Object obj) {
        if(obj instanceof Chesspiece) {
            Chesspiece piece = (Chesspiece)obj;

            if(this.x == piece.x && this.y == piece.y) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return super.equals(obj);
        }
    }
}
