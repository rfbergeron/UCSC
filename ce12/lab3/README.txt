--------------------------------
LAB 3: LOGIC UNIT WITH MEMORY
CMPE 012 Spring 2018

Robert Bergeron, 1620637
Section 01D, Rebecca Rashkin
--------------------------------


----------------
LEARNING

With this lab, I learned how to appropriately use flip-flops in MML,
how to construct a multiplexor, and how to manage numerous inputs and
outputs. Constructing the multiplexor was a lot easier than I thought
it would be, as was the register, though not to the same extent. 

----------------
ISSUES

Nothing about building the circuit was especially difficult for me. The
only real struggle was preventing the board from becoming cluttered and unreadable.
It was difficult to keep track of all of the inputs and outputs and
construct circuits in a relatively clear manner.

----------------
DEBUGGING

I added lights to the outputs of the logic operations and the multiplexor, and 
two 7-segment displays to the logic selector and MUX output to make it easier
to tell which output the multiplexor was looking at.

----------------
QUESTIONS

What is the difference between a bit-wise and reduction logic operation?

Bit-wise logic operations act on each individual pair of bits from the bit-strings
given as input, comparing the most significant bits down to the least significant bits
individually. This results in a bit string of the same length as the ones supplied as
arguments. Reduction logic operations will compare all the bits in each bit string
to each other, internally, and then compare the single-bit representations of the bit
strings to each other. This results of a single bit of output, regardless of the length
of the inputs.


What operations did we implement?

We implemented bitwise logical operations.

Why might we want to use the other type of logic operations?

Reduction logic operations are useful when we only wish to know if the bit strings 
contain at least a single true value, or all true values, or none at all. For example,
if we were ORing two 4-bit inputs and simply wanted to know that one of them was nonzero,
we would use bitwise OR, since we only need one bit to be true and one bit out output.







