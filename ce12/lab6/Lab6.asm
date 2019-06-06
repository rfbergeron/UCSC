# Lab 6: Floating Point Math
# Name: Robert Bergeron
# CruzID: rbergero@ucsc.cedu
# Date: June 9, 2018

.macro mult_by_two(%register)
 li $v0,2
 mult %register,$v0
 mflo %register
 
 # Save the last bit dropped by the division
 mflo $t6
.end_macro 
.macro div_by_two(%register)
 li $v0,2
 div %register,$v0
 mflo %register
.end_macro 
 
.text
# Subroutine IsolateSign
# Isolates the sign bit of a 32-bit single precision floating
# point number
# input:	$a0 = 32-bit float
# output: 	$v0 = sign bit, right aligned
# Side effects:  
IsolateSign:
 andi $v0,$a0,0x80000000
 srl $v0,$v0,31
 jr $ra
 
# Subroutine IsolateExponent
# Isolates 8-bit biased exponent of a 32-bit single precision
# floating point number
# input:	$a0 = 32-bit float
# output: 	$v0 = exponent bits, right aligned
# Side effects: 
IsolateExponent:
 andi $v0,$a0,0x7F800000
 srl $v0,$v0,23
 jr $ra
 
# Subroutine IsolateMantissa
# Isolates 23-bit mantissa of a 32-bit single precision
# floating point number
# input:	$a0 = 32-bit float
# output: 	$v0 = mantissa bits, right aligned
# Side effects: 
IsolateMantissa:
 andi $v0,$a0,0x007FFFFF
 jr $ra
 
.text
# Subroutine PrintNewLine
# Prints a newline character
# input: none
# Side effects: prints newline
PrintNewLine:
 li $v0,4
 la $a0,_PNL_newline
 syscall
 
 jr $ra
 
.data
_PNL_newline: .asciiz "\n" 

.text 
# Subroutine CompareFloats
# Compares two floating point values A and B.
# input: 	$a0 = Single precision float A
# 	 	$a1 = Single precision float B
# output: 	$v0 = Comparison result
# Side effects: None
# Notes: Returns 1 if A>B, 0 if A==B, and -1 if A<B
# 	Uses registers $t0-$t4
CompareFloats:
 move $t0,$a0
 move $t1,$a1 

 # Store the first link for later use
 subi $sp, $sp, 4
 sw $ra, ($sp)
 
 # Compare signs first
 
 # Isolates for float A
 jal IsolateSign
 
 move $t2,$v0
 move $a0,$a1
 
 #Isolates for float B
 jal IsolateSign
 
 move $t3,$v0
 
 # Float A is pos, B neg
 blt $t2,$t3,AGreater
 # Float B is pos, A neg
 bgt $t2,$t3,BGreater
 
 # Store the sign for later use
 move $t4,$t2
 
 # Now compare exponents
 
 move $a0,$t0
 
  # Isolates for float A
 jal IsolateExponent
 
 move $t2,$v0
 move $a0,$t1
 
 #Isolates for float B
 jal IsolateExponent
 
 move $t3,$v0
 
 # Float A has larger exponent
 bgt $t2,$t3,AGreater
 # Float B has larger exponent
 blt $t2,$t3,BGreater
 
 # Now compare mantissas
 
 move $a0,$t0
 
 # Isolates for float A
 jal IsolateMantissa
 
 move $t2,$v0
 move $a0,$t1
 
 #Isolates for float B
 jal IsolateMantissa
 
 move $t3,$v0
 
 # Float A has larger mantissa
 bgt $t2,$t3,AGreater
 # Float B has larger mantissa
 blt $t2,$t3,BGreater
 
 # If totally equal:
 move $v0,$zero
 
 lw $ra,0($sp)
 addi $sp,$sp,4
 jr $ra
 
  
  AGreater:
   # If the numbers are negative, the one that has
   #  a larger mantissa/exponent is actually the 
   #  smaller one. This is accounted for here
   bnez $t4,AInvert
   
   li $v0,1
   
   lw $ra,0($sp)
   addi $sp,$sp,4
   jr $ra
 
  BGreater:
   # If the numbers are negative, the one that has
   #  a larger mantissa/exponent is actually the 
   #  smaller one. This is accounted for here
   bnez $t4,BInvert
   
   li $v0,-1
   
   lw $ra,0($sp)
   addi $sp,$sp,4
   jr $ra
   
  AInvert:
   li $t4,0
   b BGreater
   
  BInvert:
   li $t4,0
   b AGreater
  
# Subroutine AddFloats
# Adds together two floating point values A and B.
# input: $a0 = Single precision float A
# $a1 = Single precision float B
# output: $v0 = Addition result A+B
# Side effects: None
# Notes: Returns the normalized FP result of A+B
# $t0 = stores exponent A
# $t1 = stores exponent B
# $t2 = stores mantissa A
# $t3 = stores mantissa B
# $t4 = stores sign of larger number
# $t5 = stores counter
# $t6 = last dropped bit
# $t7 = zero if signs are equal
AddFloats:
 # Initialize counter
 move $t5,$zero
 
 # Store the first link for later use
 subi $sp, $sp, 4
 sw $ra, ($sp)
 
 # Put floats on the stack temporarily so we can call 
 #  CompareFloats with their signless equivalents
 subi $sp, $sp, 4
 sw $a0, ($sp)
 subi $sp, $sp, 4
 sw $a1, ($sp)
 
 # Mask out the sign
 andi $a0,0x7FFFFFFF
 andi $a1,0x7FFFFFFF
 
 jal CompareFloats
 
 # Get original arguments off the stack
 lw $a1, ($sp)
 addi $sp,$sp,4
 lw $a0, ($sp)
 addi $sp,$sp,4
 
 bgtz $v0,AGreaterAbs
 bltz $v0,BGreaterAbs
 
 # If same size, return zero if signs aren't the same
 andi $t0,$a0,0x80000000
 andi $t1,$a1,0x80000000
 
 bne $t0,$t1,ReturnZero
 
 # If signs equal, doesnt matter which we use, so go to A
 AGreaterAbs:
  move $t4,$a0
  b SkipBGreaterAbs
  
 BGreaterAbs:
  move $t4,$a1
 
 SkipBGreaterAbs:
 
 # Isolate signs and store them in temporary registers
 andi $t2,$a0,0x80000000
 srl $t2,$t2,31
 
 andi $t3,$a1,0x80000000
 srl $t3,$t3,31
 
 # Set $t7 to 1 if signs are not equal
 xor $t7,$t2,$t3
 andi $t7,$t7,0x00000001
 
 # If signs aren't equal:
 # 	Compare their unsigned values to see which is larger
 #	Save sign of larger value to mask into final total
 #	 else, if equal, reutrn zero
 # 	Proceed as normal; adjust mantissas and exponents
 #	Once exponents are equal, use subiu instead of addiu,
 #	 subtracting smaller value from larger and ignoring signs
 #	Normalize, adding sign as usual during the final step:
 
 # Temporarily store full numbers in $t2 and $t3
 move $t2,$a0
 move $t3,$a1
 
 # Keep an extra copy of $a0 in $t6 so it can be restored for later
 #  comparison
 move $t6,$t2
 
 jal IsolateExponent
 move $t0,$v0
 
 move $a0,$t3
 jal IsolateExponent
 
 move $t1,$v0
 
 move $a0,$t2
 jal IsolateMantissa
 
 # Return float A and B to their argument slots
 move $a0,$t2
 move $a1,$t3
 
 # Add in the leading 1
 ori $t2,$v0,0x00800000
 
 
 move $a0,$t3
 jal IsolateMantissa
 
 # Add leading 1
 ori $t3,$v0,0x00800000
 
 # Restore $a1
 move $a0,$t6
 
 # Determine which number has a greater exponent
 bgt $t0,$t1,AExpGreater
 blt $t0,$t1,BExpGreater
 b ExpEqual
 
 # Increment A's mantissa and decrement its exponent
 # Repeat until mantissas are equal
 # 	Note: this can only be done 9 times. Do not do it more, even if 
 #	there are zeroes, as it would mess up the location of the 
 #	leading 1
 AExpGreater:
  mult_by_two($t2)
  subiu $t0,$t0,1
  addiu $t5,$t5,1
  
  beq $t0,$t1,ExpEqual
  bge $t5,8,ATooLarge
  b AExpGreater
 
 # If unable to make equal without losing bits, begin incrementing
 # 	the mantissa of the smaller number and incrementing its 
 #	exponent (loss of precision; rounding) 
 ATooLarge:
  div_by_two($t3)
  
  addiu $t1,$t1,1
  
  beq $t0,$t1,ExpEqual
  b ATooLarge
  
 # Increment B's mantissa and decrement its exponent
 # Repeat until mantissas are equal
 # 	Note: this can only be done 9 times. Do not do it more, even if 
 #	there are zeroes, as it would mess up the location of the 
 #	leading 1
 BExpGreater:
  mult_by_two($t3)
  subiu $t1,$t1,1
  addiu $t5,$t5,1
  
  beq $t0,$t1,ExpEqual
  bge $t5,8,BTooLarge
  b BExpGreater
  
 # If unable to make equal without losing bits, begin incrementing
 # 	the mantissa of the smaller number and incrementing its 
 #	exponent (loss of precision; rounding)
 BTooLarge:
  div_by_two($t2)
  
  addiu $t0,$t0,1
  
  beq $t0,$t1,ExpEqual
  b BTooLarge
  
 ExpEqual:
  # After this operation, $t2 and $t3 are free to use
  beqz $t7,SameSign
  
  bne $t4,$a0,BMinusA
  
  # If A is the greater number:
  subu $v0,$t2,$t3
  b SkipAdd
  
  # If B is the greater number:
  BMinusA:
  subu $v0,$t3,$t2
  b SkipAdd
  
  SameSign:
  addu $v0,$t2,$t3
  SkipAdd:
  
  # Clear registers, since they are now free too use
  move $t2,$zero
  move $t3,$zero
  
  # Mask out the sign bit from $t4
  andi $t4,0x80000000
  
  # Shrink to 23 bits using shift right, saving the dropped bits as we go
  ShrinkLoop:
   # Stop when all bits, except the leading 1, are in the correct position
   andi $t2,$v0,0xFF000000
   beq $t2,$zero,RoundAndNormalize
   
   andi $t3,$v0,0x00000001
   srl $v0,$v0,1
   addiu $t0,$t0,1
   
   b ShrinkLoop
   
 RoundAndNormalize:
  # Round if necessary
  addu $v0,$v0,$t3
  
  # Reduce again if rounding makes the number too large
  andi $t2,$v0,0xFF000000 
  bne $t2,$zero,ShrinkLoop
  
  # Mask out leading 1
  andi $v0,0xFF7FFFFF
  
  # Shift exponent in correct position
  sll $t0,$t0,23
  
  # Pack result
  or $v0,$v0,$t4
  or $v0,$v0,$t0
  
 lw $ra, ($sp)
 addi $sp, $sp, 4
 
 jr $ra
 
 # Returns zero, duh
 ReturnZero:
  move $v0,$zero
 
  lw $ra, ($sp)
  addi $sp, $sp, 4
  
  jr $ra
  
# Subroutine NormalizeFloat
# Normalizes, rounds, and “packs” a floating point value.
# input: $a0 = 1-bit Sign bit (right aligned)
# $a1 = [63:32] of Mantissa
# $a2 = [31:0] of Mantissa
# $a3 = 8-bit Biased Exponent (right aligned)
# output: $v0 = Normalized FP result of $a0, $a1, $a2
# Side effects: None
# Notes: Returns the normalized FP value by adjusting the
# exponent and mantissa so that the 23-bit result
# mantissa has the leading 1(hidden bit). More than
# 23-bits will be rounded. Two words are used to
# represent an 18-bit integer plus 46-bit fraction
# Mantissa for the MultFloats function. (HINT: This
# can be the output of the MULTU HI/LO registers!)
NormalizeFloat:

 # Store original call address
 subiu $sp,$sp,4
 sw $ra, ($sp)
 
 # If exponent and mantissa are zero, branch to the end
 or $t0,$a1,$a2
 or $t0,$t0,$a3
 beqz $t0,NoCarry
 
 # Load a 2 for dividing
 li $t0,2
 
 # Instead of just decrementing until it fits a 23 bit space,
 #  need to account for the fact that the first 18 bits will
 #  always be integers. So, instead, only shift right/divide
 #  until the first 17 bits are zero, adding to the exponent
 #  every time. Then, mask out the rightmost 14 bits of $a1
 #  and leftmost 9 bits of $a2. Or them into $v0, round and
 #  normalize again if necessary, then or the exponent and sign
 #  into the $v0 register as well
 # If no integer bits, use left shifts until there is a leading
 #  one in the expected position. Use left shifts on $a1 and 
 #  mult on $a2 to make use of the hi register, oring its
 #  contents into $a1
 ShiftIntegerLoop:
  andi $t6,$a1,0xFFFFC000
  
  # Branch here if no leading one
  beqz $t6,NormalizeLoop
  
  # Branch here if there is a leading one in the expected position
  beq $t6,0x00004000,IntegerReduced
  
  # Divide upper bits to shift left and keep remainder
  divu $a1,$t0
  mflo $a1
  
  # Shift dropped upper bit all the way to the left
  mfhi $t1
  sll $t1,$t1,31
  
  # Or the dropped bit into the lower register
  srl $a2,$a2,1
  or $a2,$a2,$t1
  
  # Increase exponent by one to compensate for shift
  addiu $a3,$a3,1
  
  b ShiftIntegerLoop
  
 NormalizeLoop:
  # Shift $a1 right
  srl $a1,$a1,1
  
  # Multiply $a2 by two; or hi bit into $a1
  mult $a2,$t0
  mflo $a2
  mfhi $t1
  or $a1,$a1,$t1
  
  # Subtract from exponent to account for shift
  subiu $a3,$a3,1
  
  # Mask to check for leading 1
  andi $t6,$a1,0x00800000
  
  # Branch away if there is now a leading 1
  beq $t6,0x00800000,IntegerReduced

  b NormalizeLoop
  
 IntegerReduced:
  
  # Mask out last 14 bits of $a1 (leading 1 is NOT included)
  andi $v0,$a1,0x00003FFF
  
  # Shift 14 bits over 9 positions to make room for $a2's bits
  sll $v0,$v0,9
  
  # Mask out leftmost 9 bits of $a2; shift to correct position
  andi $t1,$a2,0xFF800000
  srl $t1,$t1,23
  
  # Or together $a1 and $a2 bits
  or $v0,$v0,$t1
  
  # Retrieve last dropped bit for rounding
  andi $t1,$a2,0x00400000
  srl $t1,$t1,22
  
  # Add rounding bit
  addu $v0,$v0,$t1
  
  # Branch away if rounding did not cause a carryout into
  #  the 24th bit
  andi $t1 $v0,0xFF800000
  beqz $t1,NoCarry
  
  # We have already accounted for the leading one. So, if there
  #  was a carryout bit, the actual value of the bitstring ought
  #  to be something like 0b 0000 0001 0000 ... but instead we
  #  see 0b 0000 0000 1000 ... The value has already been shifted
  #  so all that is necessary is to mask out the value and account
  #  for the shift in the exponent
  andi $v0,$v0,0x007FFFFF
  addiu $a3,$a3,1
 
 NoCarry:
 
 # Shift sign and exponent bits
 sll $a0,$a0,31
 sll $a3,$a3,23
 
 # Or in the sign and exponent
 or $v0,$v0,$a0
 or $v0,$v0,$a3
 
 lw $ra, ($sp)
 addiu $sp,$sp,4
 
 jr $ra
 
# Subroutine MultFloats
# Multiplies two floating point values A and B.
# input: $a0 = Single precision float A
# $a1 = Single precision float B
# output: $v0 = Multiplication result A*B
# Side effects: None
# Notes: Returns the normalized FP result of A*B
MultFloats:

 subiu $sp,$sp,4
 sw $ra, ($sp)
 
 # Mask out float A mantissa and add leading 1
 andi $t0,$a0,0x007FFFFF
 ori $t0,$t0,0x00800000
 
 # Do the same for float B
 andi $t1,$a1,0x007FFFFF
 ori $t1,$t1,0x00800000
 
 multu $t0,$t1
 mfhi $t2
 mflo $t3
 
 andi $t0,$a0,0x7F800000

 andi $t1,$a1,0x7F800000
 
 # Put exponent into its proper register since its not being used
 addu $a3,$t0,$t1
 srl $a3,$a3,23
 
 # Subtract 127 from exponent
 subiu $a3,$a3,127
 
 andi $t0,$a0,0x80000000
 andi $t1,$a1,0x80000000
 
 # Put sign directly into argument register since $a0 is no longer needed
 xor $a0,$t0,$t1
 srl $a0,$a0,31
 
 # Move both halves of mantissa product into proper registers
 move $a1,$t2
 move $a2,$t3

 # Normalize now that arguments are full
 jal NormalizeFloat
 
 # Return value is already in $ra, so jump back to call point
 lw $ra, ($sp)
 addiu $sp,$sp,4
 
 jr $ra

.text
# Subroutine PrintFloat
# Prints the sign, mantissa, and exponent of a SP FP value.
# input: $a0 = Single precision float
# Side effects: None
# Notes: See the example for the exact output format. 
PrintFloat:
 subiu $sp,$sp,4
 sw $ra, ($sp)
 
 move $t0,$a0
 
 la $a0,Sign
 li $v0,4
 syscall
 
 andi $a0,$t0,0x80000000
 srl $a0,$a0,31
 addiu $a0,$a0,0x30
 
 li $v0,11
 syscall
 
 jal PrintNewLine
 
 # Mask out exponent, align right, set counter register to zero
 andi $t1,$t0,0x7F800000
 srl $t1,$t1,23
 move $t2,$zero
 
 # Print exponent label
 la $a0,Exponent
 li $v0,4
 syscall
 
 StackExponent:
  andi $a0,$t1,0x00000001
  addiu $a0,$a0,0x30
  
  srl $t1,$t1,1
  
  subiu $sp,$sp,4
  sw $a0, ($sp)
 
  addiu $t2,$t2,1
  
  blt $t2,8,StackExponent
  
 PrintExponent:
  lw $a0, ($sp)
  addiu $sp,$sp,4
  li $v0,11
  syscall
  subiu $t2,$t2,1
  
  bgtz $t2,PrintExponent
  
 # Reset the counter
 move $t2,$zero
  
 jal PrintNewLine
 
 # Print mantissa label
 la $a0,Mantissa
 li $v0,4
 syscall
 
 StackMantissa:
  andi $a0,$t0,0x00000001
  addiu $a0,$a0,0x30
  
  srl $t0,$t0,1
  
  subiu $sp,$sp,4
  sw $a0, ($sp)
 
  addiu $t2,$t2,1
  
  blt $t2,23,StackMantissa
  
 PrintMantissa:
  lw $a0, ($sp)
  addiu $sp,$sp,4
  li $v0,11
  syscall
  subiu $t2,$t2,1
  
  bgtz $t2,PrintMantissa
 
 lw $ra, ($sp)
 addiu $sp,$sp,4
 
 jr $ra

.data
Sign: .asciiz "SIGN: "
Exponent: .asciiz "EXPONENT: "
Mantissa: .asciiz "MANTISSA: "

 # Store the last dropped bit
 # Add once they are equal
 # Round (if last dropped bit was 1, add to last bit of mantissa

# Subroutine 
# 
# input: 
# Side effects:  
