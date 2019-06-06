# Lab 5: Hex to Decimal Conversion
# Name: Robert Bergeron
# CruzID: rbergero@ucsc.cedu
# Date: May 30, 2018
# 
# 
# Pseudocode:
# 
# print("Input a hex number:\n"
# 
# currentAddress = $a0
# currentChar = load(currentAddress)
# stringLength = 0
# 
# while(currentChar > 0) {
#  stringLength++
#  currentAddress += 4
#  currentChar = load(currentAddress)
# }
# 
# hexPlace = 1
# 
# while(stringLength > 0) {
#  if(currentChar is a letter) {
#   subtract 55
#  }
# 
#  if(currentChar is a number) {
#   subtract 48
#  }
#  
#  if(currentChar us a lowercase x) {
#   break out of loop
#  }
#  
#  stringLength--;
#  currentAddress -= 4;
#  currentChar = load(currentAddress)
# }

.text 
main: # Program starts here
 
 move $t0,$a1
 
 # Prompt for hex number
 li $v0,4 
 la $a0,prompt
 syscall 
 
 # Load the location of the input string from memory
 lw $t0,0($t0)
 li $t3,1
 
 # Prints the hex number
 move $a0,$t0
 syscall
 
 # Adds newline after the hex number
 la $a0,newline
 syscall
  
 b findlength
 
 # Load halt program function and call it
 end:
  li $v0,10
  syscall 
  
 # Iterates to the end of the hex number in memory, so that it can be gone back over
 # in reverse
 findlength:
  lb $t2,0($t0)
  
  beqz  $t2,loadargs
  
  addiu $t1,$t1,1
  
  addiu $t0,$t0,1
  
  b findlength
  
 # Here, $t3 will serve as the power of 16 to multiply by.
 # $t4 will store the integer representation of the hex number.
 # $t5 is 16, used for multiplication of $t3
 # 
 # Traverses the hex number, from least to most significant place, adding it to $s0 
 # and $t4
 loadargs:
  subiu $t0,$t0,1
  
  subiu $t1,$t1,1
  lb $t2,0($t0)
  
  beq $t2,120,queuenullchar
  blt $t2,65,hexnumber
  b hexletter
  
 # Prints header for decimal number and adds a null character to the stack
 # to denote the end of the string representation of the decimal 
 queuenullchar:
  
  li $t5,0
  subi $sp,$sp,4
  lw $t5,0($sp)
  
  la $a0,decimalheader
  li $v0,4
  syscall
  
  b remainders
  
 postadjust:
  
  # multiplies the digit by a power of 16 so that it goes into the correct place
  mult $t2,$t3
  
  mfhi $t5
  mflo $t6
  
  # Add the number to the stored and working representations
  add $t4,$t4,$t6
  add $s0,$s0,$t6
  
  # Get the $t3 register ready for the next appropriate hexadecimal place
  li $t5,16
  mult $t3,$t5
  mflo $t3
  
  b loadargs
  
 
 # Shifts char to its appropriate decimal number, if it was an arabic numeral
 hexnumber:
  subiu $t2,$t2,0x30
  
  b postadjust
 
 # Shifts the char to its appropriate decimal number, if it
 # was an A B C D E or F
 hexletter:
  subiu $t2,$t2,0x37
  
  b postadjust
  
 # Converts negative numbers to their unsigned representation
 tounsigned:
  subiu $t5,$t5,1
  not $t5,$t5
  
  b postunsigned
 
 # Uses remainders from division to get the appropriate digit for each decimal place,
 # and breaks away once the most significant place has been reached (no dividend)
 remainders:
  li $t5,10
  
  div $t4,$t5
  
  mflo $t4
  mfhi $t5
  
  # If there is no dividend, the most significant digit has been reached
  beqz $t4,significantdigit
  
  # if the remainder is negative, convert
  bltz $t5,tounsigned
  
  postunsigned:
  
  # Convert the decimal back to its character representation
  addiu $t5,$t5,0x30
  
  # Add it to the stack
  subiu $sp,$sp,4
  sw $t5,0($sp)
   
  
  b remainders
  
  significantdigit:
   # If the remainder/number is not negative, skip the conersion
   bgez $t5,noconversion
   
   # Conert to unsigned
   subiu $t5,$t5,1
   not $t5,$t5
   
   # Shift to character
   addiu $t5,$t5,0x30
   
   # Add it to the stack
   subiu $sp,$sp,4
   sw $t5,0($sp)
   
   # Add a minus sign to the stack as well
   li $t5,0x2D
   subiu $sp,$sp,4
   sw $t5,0($sp)
   
   b printstack
   
   # Break here if the number was positive
   noconversion:
   
   addiu $t5,$t5,0x30
   
   subiu $sp,$sp,4
   sw $t5,0($sp)
   
   b printstack
   
  printstack:
   li $v0,4
   move $a0,$sp
   syscall
   
   sw $zero,0($sp)
   addi $sp,$sp,4
   
   lw $t0,0($sp)
   beqz $t0,end
   
   b printstack
  
.data # Define the program data.
prompt: .asciiz "Input a hex number:\n" 
newline: .asciiz "\n"
decimalheader: .asciiz "The decimal value is:\n"
