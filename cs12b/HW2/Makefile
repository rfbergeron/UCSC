#------------------------------------------------------------------------------
# Makefile for HW2
# Robert Bergeron
# rbergero
#------------------------------------------------------------------------------
JAVASRC = Rook.java Bishop.java King.java Queen.java Chesspiece.java Chessboard.java Node.java Color.java
SOURCES = README Makefile $(JAVASRC)
MAINCLASS = Chessboard
CLASSES = Rook.class Bishop.class King.class Queen.class Chesspiece.class Chessboard.class Node.class Color.class
JARFILE = ChessBoard.jar
all: $(JARFILE)

$(JARFILE): $(CLASSES)
	echo Main-class: $(MAINCLASS) > Manifest
	jar cvfm $(JARFILE) Manifest $(CLASSES)
	rm Manifest
	
$(CLASSES): $(JAVASRC)
	javac -Xlint $(JAVASRC)
	
clean:
	rm $(CLASSES) $(JARFILE)

