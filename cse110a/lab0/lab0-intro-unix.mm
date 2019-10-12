.so Tmac.mm-etc
.if t .Newcentury-fonts
.INITR* \n[.F]
.SIZE 12 14
.TITLE Lab0 "Introduction to Unix"
.RCS "$Id: lab0-intro-unix.mm,v 1.22 2019-09-11 13:04:11-07 - - $"
.PWD
.URL
.nr SHELLCNT 0 1
.de COMMAND
.   ft CR
.   nop -bash-\\n+[SHELLCNT]$
.   ft CB
.   nop \\$*
.   ft CR
.   br
.   nf
.   tm \\$*
.   pso \\$* | expand
.   br
.   ft R
.   fi
..
.P
It is assumed that everyone in class is familiar with Unix (Linux),
the UCSC
.V= submit
command, and using
.V= Makefile s.
.P
TAs should review the use of the Linux command line for those
few students who might not be familiar with it.  All programming
projects in this course will be done on the
.V= unix.ucsc.edu
servers or the workstations in BE/105.
.P
Most students will, of course, develop on their own computers.
However, the production environment will be the Linux servers,
and all code must run there.
.P
Use of the
.V= submit
command\(::
All programs will be submitted via submit on the campus
Linux hosts\(::
.VTCODE* 1 "submit cmps110a-wm.f19 lab0 \fIfiles\|.\|.\|.\fP"
Substitute for lab0 the names of later assignments.
.P
Accessing the command line from home.
.VTCODE* 1 "ssh -X \fIusername\fP@unix.ucsc.edu"
The option
.V= -X
works if you are doing X11 forwarding.
.P
Using
.V= make \(::
Write a simple program in C++ using several modules,
header files, and separate compilation.
Write a
.V= Makefile
that can build it.
Submit the source code and
.V= Makefile
as
.V= lab0 .
.P
While we will be using C++ in this course, all students are
expected to know C (prerequisite),
and must learn just enough C++ to complete the projects.
Source code files should have the suffices
.V= \&.h
or
.V= \&.cpp ,
and use
.V= g++
to compile source code.
.P
This lab will not be graded.
There is no credit for it.
If the above interaction with Linux is not completely obvious,
attend the first couple of labs to be brought up to speed by the TA.
.P
See the
.V= code/
subdirectory for the solution to this lab and a sample of how
to mix
.V= flex ,
.V= bison ,
and C++.
The subdirectory
.V= code/
shows a main program and a hello class and
illustrates various features of
C++ that will be covered later in the course.
.P
If you are doing development on your own computer,
use the latest version of
.V= g++
or a newer version.
Any code you develop must compile and run on
.V= unix.ucsc.edu ,
since that is the only environment the graders will be using.
Be sure that the compiler you are using is at least C++14 compliant.
.P
.COMMAND which g++
.COMMAND g++ --version | grep -i g++
