# $Id: Makefile,v 1.13 2019-02-28 15:24:20-08 - - $
# John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)

MKFILE     = Makefile
DEPFILE    = ${MKFILE}.dep
NOINCL     = ci clean spotless
NEEDINCL   = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE      = ${MAKE} --no-print-directory
FORMATCPP  = clang-format --style=file -i

GPPWARN     = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++17 -MM ${GPPOPTS}

MODULES    = graphics interp rgbcolor shape debug util main
CPPSOURCE  = $(wildcard ${MODULES:=.cpp})
GENFILES   = colors.cppgen
MODFILES   = ${foreach MOD, ${MODULES}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
SOURCES    = ${wildcard ${MODFILES}}
OTHERS     = mk-colors.perl ${MKFILE} ${DEPFILE}
ALLSOURCES = ${SOURCES} ${OTHERS}
EXECBIN    = gdraw
OBJECTS    = ${CPPSOURCE:.cpp=.o}
LINKLIBS   = -lGL -lGLU -lglut -ldrm -lm
LISTING     = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS} ${LINKLIBS}

%.o : %.cpp
	${COMPILECPP} -c $<

colors.cppgen: mk-colors.perl
	perl mk-colors.perl >colors.cppgen

ci : ${ALLSOURCES}
	- git add ${SOURCES} ${MKFILE}

clean :
	- rm -f ${OBJECTS} ${DEPFILE} core ${GENFILES}

spotless : clean
	- rm -f ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf} ${DEPFILE}

dep : ${CPPSOURCE} ${GENFILES}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} :
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all

format :
	${FORMATCPP} ${SOURCES}

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif

