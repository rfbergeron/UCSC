class Chessboard {
    //top left corner of the board
    BoardSpace entryPoint;

    public Chessboard(int size) {
        entryPoint = new BoardSpace(true);
        entryPoint.addVertical(size, this, true);
        entryPoint.addHorizontal(size, this, true);

        Boardspace current = entryPoint;

        do {
            current = current.nextHorizontal;
            current.addVertical(size, this, false);
        } while(current.nextHorizontal != entryPoint);

        current = entryPoint;

        do {
            current.addRightDiagonal(size, this);
            current.addLeftDiagonal(size, this);
            current = current.nextHorizontal;
        } while(current != entryPoint);


    }

    void addQueens() {

    }

}