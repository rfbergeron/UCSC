--------------------------------
LAB 6: FLOATING POINT MATH
CMPE 012 Spring 2018

Robert Bergeron, 1620637
Section 01D, Rebecca Rashkin
--------------------------------


----------------
LEARNING

This lab took an insane amount of time. I feel that now I have a very clear
understanding of how floating point decimals are represented in binary. 
Additionally, I have learned the value in not simply jumping into a coding 
project, as the mess I have created is, while readable, far too difficult
to cleanup in a way that would not cause me a headache. Also it's like 4AM
and I'm just happy this demon of a lab is functional. Though I did not write
a large amount of test code, the test code I did write, which simply uses 
different numbers, has helped me pick out terrible bugs in my code that I
would not otherwise have caught.

----------------
ANSWERS TO QUESTIONS

1. The only test code I "wrote" was in run_Lab6 and was basically just a copy 
	of Lab6_test with more/different numbers. Since edge cases are not really
	being tested, I wanted to make sure that the functions work with random 
	numbers.

2. Floating point overflow is different from integer overflow. When a number is
	too big or small to be represented, instead of jumping to a wildly different
	value, it will become a symbolic value close to what the actual value could be.
	For example, take a float with a 0 sign bit, all 1's in the exponent save for 
	the rightmost bit, and all 1's in the mantissa. If 1 is added to this value, 
	the mantissa carries out into the exponent, and so must be shifted right and
	the exponent incremented. However, the exponent then becomes 255, which is
	normally used to represent infinity. This is not the desired value, or even
	a useful one, but it does indicate a value too large to be given a precise
	representation in floating point form.

3. Rounding seemed to be a relatively simple issue. Just save the last bit dropped,
	which happens anyways if you use divison to shift, and add it into the number 
	after you've finished shrinking the mantissa. If the mantissa carries out into 
	the exponent after rounding, just mask out the extra one and add 1 to the
	exponent to account for the shift in place.


4. I wrote additional functions which isolate and return different portions of 
	a normalized floating point value, as well as a function the simply prints a 
	newline. In hindsight, both of these would be less irritating to use if they
	were simply macros, but I didn't bother learning how to write macros at the
	time I coded them, as I was more interested in honing my knowledge of functions.

