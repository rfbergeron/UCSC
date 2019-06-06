#------------------------------------------------------------------------------
# Makefile for HW3
# Robert Bergeron
# rbergero
#------------------------------------------------------------------------------
JAVASRC = Stack.java Entry.java
SOURCES = README Makefile $(JAVASRC)
MAINCLASS = Stack
CLASSES = Stack.class Entry.class
JARFILE = Balanced.jar
all: $(JARFILE)

$(JARFILE): $(CLASSES)
	echo Main-class: $(MAINCLASS) > Manifest
	jar cvfm $(JARFILE) Manifest $(CLASSES)
	rm Manifest
	
$(CLASSES): $(JAVASRC)
	javac -Xlint $(JAVASRC)
	
clean:
	rm $(CLASSES) $(JARFILE)

