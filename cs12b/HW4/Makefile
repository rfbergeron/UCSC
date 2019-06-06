#------------------------------------------------------------------------------
# Makefile for HW4
# Robert Bergeron
# rbergero
#------------------------------------------------------------------------------
JAVASRC = RefQueue.java QueueEvent.java Node.java
SOURCES = README Makefile $(JAVASRC)
MAINCLASS = RefQueue
CLASSES = RefQueue.class QueueEvent.class Node.class
JARFILE = SlugFest.jar
all: $(JARFILE)

$(JARFILE): $(CLASSES)
	echo Main-class: $(MAINCLASS) > Manifest
	jar cvfm $(JARFILE) Manifest $(CLASSES)
	rm Manifest
	
$(CLASSES): $(JAVASRC)
	javac -Xlint $(JAVASRC)
	
clean:
	rm $(CLASSES) $(JARFILE)

