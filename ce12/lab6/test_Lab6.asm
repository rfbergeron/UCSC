# test_Lab6.asm
# Date:  4/18/2018
# Author:  mrg
# This code calls the functions from Lab6.asm.

#-------helpful macros to improve readability:-------
.macro sys(%which_sys)
	li $v0, %which_sys
	syscall
.end_macro
.macro print_float(%register)
	mtc1 %register, $f12
	li $v0, 2
	syscall
.end_macro
.macro print_int(%register)
	move $a0, %register
	li $v0, 1
	syscall
.end_macro
.macro print_str(%label)
	la $a0, %label
	li $v0, 4
	syscall
.end_macro
.macro print_char(%register)
	move $a0, %register
	li $v0, 11
	syscall
.end_macro


#-------Main testing code for Lab6.asm-------
.text
main: nop
	jal testPrint   # A function to test student's PrintFloat
	nop
	
	jal testCompare # A function to test student's CompareFloats
	nop

	jal testAdd     # A function to test student's AddFloats
	nop
	
	jal testMult    # A function to test student's MultFloats
	nop
	
	sys(10) #exit


#-------Functions for individual tests:-------
# Function testPrint
# input:	None
# output:  	None
# Side effects: Prints report of results of PrintFloat
#		Also overwrites $a0 
.data
test_A:	.float -0.9
test_B:	.float 0.5
testPrint_introstr: 	.asciiz  "\n\nPrintFloat Test \n"
testCompare_introstr: 	.asciiz  "\n\nCompareFloat Test \n"
testAdd_introstr: 	.asciiz  "\n\nAddFloat Test \n"
testMult_introstr: 	.asciiz  "\n\nMultFloat Test \n"
test_newline:		.asciiz  "\n"
.text
testPrint: nop
	subi $sp, $sp, 4
	sw $ra, ($sp)

	print_str(testPrint_introstr) #print test description
	
	lw  $a0, test_A
	
	la    $t9, PrintFloat
  	jalr  $t9  #call student's function
	
	lw $ra, ($sp)
	addi $sp, $sp, 4
	
	jr $ra
	
testCompare: nop
	subi $sp, $sp, 4
	sw $ra, ($sp)
	
	print_str(testCompare_introstr) #print test description
	
	lw  $a0, test_A
	lw  $a1, test_B
		
	la    $t9, CompareFloats
  	jalr  $t9  #call student's function
	
	print_int($v0) #print result -1 (less than)
	print_str(test_newline) 
	
	lw  $a0, test_B
	lw  $a1, test_A
		
	la    $t9, CompareFloats
  	jalr  $t9  #call student's function
	
	print_int($v0) #print result 1 (greater than)
	print_str(test_newline) 
		
	lw  $a0, test_A
	lw  $a1, test_A
		
	la    $t9, CompareFloats
  	jalr  $t9  #call student's function
	
	print_int($v0) #print result 0 (equal)

	lw $ra, ($sp)
	addi $sp, $sp, 4

	jr $ra
	
testAdd: nop
	subi $sp, $sp, 4
	sw $ra, ($sp)
	

	print_str(testAdd_introstr) #print test description
	
	lw  $a0, test_A
	lw  $a1, test_B
		
	la    $t9, AddFloats
  	jalr  $t9  #call student's function
	
	print_float($v0) #print result -0.39999998
	
	lw $ra, ($sp)
	addi $sp, $sp, 4

	jr $ra
	
testMult: nop
	subi $sp, $sp, 4
	sw $ra, ($sp)

	print_str(testMult_introstr) #print test description
	
	lw  $a0, test_A
	lw  $a1, test_B
		
	la    $t9, MultFloats
  	jalr  $t9  #call student's function
	
	print_float($v0) #print result -0.45
	
	lw $ra, ($sp)
	addi $sp, $sp, 4

	jr $ra
		
nop #---------------------END CALLING CODE-------------------------------------------------
nop
nop #---------------------BEGINNING OF LAB6 CODE-------------------------------------------
#student code is included here:
.include "Lab6.asm"
