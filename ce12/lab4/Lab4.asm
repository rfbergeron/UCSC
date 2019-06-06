# Lab4 4: FEEDBABE in MIPS
# Name: Robert Bergeron
# CruzID: rbergero@ucsc.cedu
# Date: May 12, 2018
.text 
main: # Program starts here
 
 # t0 holds the target value
 # t1 holds the counter
 # t3 holds the number 3
 # t4 holds the number 4
 # t5 holds the remainder of divison by 3
 # t6 holds the remainder of division by 4
 
 # Prompts user to input a number
 li $v0,4 
 la $a0,prompt 
 syscall 
 
 # Calls instruction to read user input
 li $v0,5
 syscall
 
 # Move input into temp regsiter
 move $t0,$v0
 
 # Start counter at 1
 li $t1,1
 
 # Set register t3 and t4 to 3 and 4 for use with div operator
 li $t3,3
 li $t4,4
 
 # Enter loop
 b loop
 
 
 # Load halt program function and call it
 end:
  li $v0, 10
  syscall 
  
 loop:
  
  # Get remainder for division by three and branch if there is none
  div $t1,$t3
  mfhi $t5
  beq $t5,$zero,printfeed
  
  postfeed:
  
  # Get remainder for division by four and branch if there is none
  div $t1,$t4
  mfhi $t6
  beq $t6,$zero,printbabe
  
  # Skips printing integer if number was divisible by 3 and not 4
  beq $t5,$zero,postprint
  
  # Load the counter's number into argument register and print as integer
  li $v0,1
  move $a0,$t1
  syscall
  
  postprint:
  
  # Print new line character
  li $v0,4
  la $a0,newline
  syscall
  
  # Increment counter
  addi $t1,$t1,1
  
  # Branch if counter has reached desired value
  bgt $t1,$t0,end
  
  # Else, continue loop
  b loop
 
 # Prints feed, then returns to the line immediately 
 # following the conditional
 printfeed:
  li $v0,4
  la $a0,feed
  syscall
  
  b postfeed
 
 # Prints babe, then skips to the newline print
 # statement, since we have already checked for division by three
 # and confirmed that the number is divisible by four, so we do
 # not need to print an integer  
 printbabe:
  li $v0,4
  la $a0,babe
  syscall
  
  b postprint
 
.data # Define the program data.
prompt: .asciiz "Please input a number: " 
newline: .asciiz "\n"
feed: .asciiz "FEED"
babe: .asciiz "BABE"
