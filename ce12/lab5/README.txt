--------------------------------
LAB 5: HEX TO DECIMAL CONVERSION
CMPE 012 Spring 2018

Robert Bergeron, 1620637
Section 01D, Rebecca Rashkin
--------------------------------


----------------
LEARNING

As I work with MIPS its becoming clear that I tend to overthink how to
execute a task, such as using a stack. However, I do tend to undethink 
when it comes to actually planning out a program. I have become used
to annotating my code poorly, which makes understanding what I've done
previously in my program far harder than it needs to be. 

----------------
ANSWERS TO QUESTIONS

1. In the case of number like 0xDEADBEEF and 0x000000F0, each have only
   one representation of zero. In 0xFEEDBABE, there are no representations
   of zero.

2. The largest input value the program supports is 2147483647


3. The smallest input value the program supports is -2147483648


4. In mips, unsigned operations treat the bits as though they are an unsigned
   number. Any sequence of bits can be used for both signed and unsigned
   operations. Unsigned operations, however, do not have overflow, which
   means that if MIPS attempts to store values that are too large into
   the registers, MIPS will not complain, which could potentially cause some
   very strange arithmetic errors.


5. The code for a binary to decimal converter would be similar to a hex
   converter, except simpler. The input string would be read, from least
   to most significant bits, subtracting 30 from the character representation
   to get the actual number, which would be multiplied by a power of 2 according
   to its place. This number would be added to a register for storage. From that
   point on, the program is identical to this one; just divide by powers of ten,
   add the remainders to the stack, and then pull them at the end.

