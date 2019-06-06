--------------------------------
LAB 4: FEEDBABE IN MIPS
CMPE 012 Spring 2018

Robert Bergeron, 1620637
Section 01D, Rebecca Rashkin
--------------------------------


----------------
LEARNING

I learned that using MIPS is not as confusing or as frustrating as I thought it would be.
While working with memory addressess directly and very little abstraction can be confusing
at times, assembly is also so simple that performing basic operations is very simple since
you dont have to worry about access restrictions or data types. All you need to keeep track
of is which registers are in use. Labels are also easy to use and make for interesting
conditionals, since the assembler can keep reading beyond them, unlike java, where methods
have a definitive end. 


----------------
ANSWERS TO QUESTIONS

1.
N can be as large as 2^32 - 1, since N is stored in a single 32-bit register.

2.
The characters of the prompt string are stored in the range of addresses between
0x10010000 and 0x10010016.

3.
The move, load immediate, unconditional branch, and branch if greater than functions are
all pseudo ops. The move and load immediate pseudo-ops use the add operation; unconditional
branch checks if zero equals zero, and bgt uses the "set on less than" operation, as well as
the "branch on not equal" operation.

4.
I used regsiters t1 through t6, except for t2. I could have used less had I used a single
register to hold the divisor and swapped it between 3 and 4. 
