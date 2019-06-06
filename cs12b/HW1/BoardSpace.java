
class BoardSpace {
    boolean hasQueen;
    boolean isHead;
    BoardSpace nextHorizontal;
    BoardSpace nextVertical;
    BoardSpace nextRightDiagonal;
    BoardSpace nextLeftDiagonal;
    
    public BoardSpace(boolean headSpace) {
        this.hasQueen = false;
        this.isHead = headSpace;
    }

    void addRightDiagonal(int numToAdd, BoardSpace head) {
         BoardSpace hor = this.nextHorizontal;

         if(hor != null && hor.nextVertical != null) {
             this.nextRightDiagonal = hor.nextVertical;
             this.nextRightDiagonal.addDiagonal(numToAdd - 1, head);
         }
         else {
             this.nextRightDiagonal = head;
         }
    }

    void addLeftDiagonal(int numToAdd, BoardSpace head) {
        BoardSpace vert = this.nextVertical;
        BoardSpace hor = vert.nextHorizontal;

        if(!vert.isHead) {
            //iterates through the linked list, wrapping around to the space before vert
            while(hor.nextHorizontal != vert) {
                hor = hor.nextHorizontal;
            }

            this.nextLeftDiagonal = hor;
            this.nextLeftDiagonal.addLeftDiagonal(numToAdd, head);
        }
        else {
            this.nextLeftDiagonal = head;
        }
    }



    void addHorizontal(int numToAdd, BoardSpace head, boolean headSpace) {
        if(numToAdd > 0) {
            this.nextHorizontal = new BoardSpace(headSpace);
            nextHorizontal.addHorizontal(numToAdd - 1, head);
        }
        else {
            this.nextHorizontal = head;
        }
    }

    void addVertical(int numToAdd, BoardSpace head, boolean headSpace) {
        if(numToAdd > 0) {
            this.nextVertical = new BoardSpace(headSpace);
            nextVertical.addVertical(numToAdd - 1, head);
        }
        else {
            this.nextVertical = head;
        }
    }


}