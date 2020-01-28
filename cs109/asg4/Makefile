# John Gnanasekaran (jgnanase) and Robert Bergeron (rbergero)
# $Id: Makefile,v 1.12 2019-02-21 18:18:42-08 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
FORMATCPP   = clang-format --style=file -i

GPPWARN     = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -std=gnu++17 -g -O0 ${GPPOPTS}
MAKEDEPCPP  = g++ -std=gnu++17 -MM

MODULES     = debug logstream protocol sockets
EXECBINS    = ${CIXEXEC} ${CIXDEXEC}
CIXEXEC		= cix
CIXDEXEC    = cixd
ALLMODS     = ${MODULES} ${EXECBINS}
SOURCELIST  = ${foreach MOD, ${ALLMODS}, ${MOD}.h ${MOD}.tcc ${MOD}.cpp}
ALLSOURCE   = ${wildcard ${SOURCELIST}} ${MKFILE}
CPPLIBS     = ${wildcard ${MODULES:=.cpp}}
OBJLIBS     = ${CPPLIBS:.cpp=.o}
CIXOBJS     = cix.o ${OBJLIBS}
CIXDOBJS    = cixd.o ${OBJLIBS}
CLEANOBJS   = ${OBJLIBS} ${CIXOBJS} ${CIXDOBJS}
LISTING     = Listing.ps

all: ${EXECBINS}

cix: ${CIXOBJS}
	${COMPILECPP} -o $@ ${CIXOBJS}

cixd: ${CIXDOBJS}
	${COMPILECPP} -o $@ ${CIXDOBJS}

%.o: %.cpp
	${COMPILECPP} -c $<

ci: ${ALLSOURCE}
	git add ${ALLSOURCE}

clean:
	- rm -f ${LISTING} ${LISTING:.ps=.pdf} ${CLEANOBJS} core

spotless: clean
	- rm -f ${EXECBINS} ${DEPFILE}

dep: ${wildcard ${SOURCELIST}}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${wildcard ${SOURCELIST}} >>${DEPFILE}

${DEPFILE}:
	@ touch ${DEPFILE}
	${GMAKE} dep

again: ${ALLSOURCE}
	${GMAKE} spotless dep ci all

format:
	${FORMATCPP} ${wildcard ${SOURCELIST}}

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif
