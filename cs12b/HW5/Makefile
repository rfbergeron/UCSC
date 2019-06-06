#------------------------------------------------------------------------------
# Makefile for HW5
# Robert Bergeron
# rbergero
#------------------------------------------------------------------------------
JAVASRC = Bard.java
SOURCES = README Makefile $(JAVASRC)
MAINCLASS = Bard
CLASSES = Bard.class
JARFILE = Bard.jar
all: $(JARFILE)

$(JARFILE): $(CLASSES)
	echo Main-class: $(MAINCLASS) > Manifest
	jar cvfm $(JARFILE) Manifest $(CLASSES)
	rm Manifest
	
$(CLASSES): $(JAVASRC)
	javac -Xlint $(JAVASRC)
	
clean:
	rm $(CLASSES) $(JARFILE)

