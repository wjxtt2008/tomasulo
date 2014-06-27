**********************************************************************
This project is conducted by Zheng Liu,Dai Yuanjun,Junxiang Wu

Universiry of Pittsburgh

ECE2162 Computer Architecture Fall 2013



Jun 26,2014 Updated by Junxiang Wu 
junxiangwu1991@gmail.com

**********************************************************************
Requirement

1.Run environment: ubuntu 12.04 64bit 
2.Compile: gcc 4.6.3
**********************************************************************
Complie

Download src, type " make " to complie
**********************************************************************
Input and Output

The program will read input under its current path and print out result to termial. Which means you need to put input and program in the same floder.

1. input.txt: Instructions.
2. config.txt: Hardware configuration, Register & Memory Initialization.
3. output.txt: Result file.

***********************************************************************
Execute Instruction 

1.complie
2.put input.txt and config.txt to the same directory with complied "tomasulo" 
3.run in terminal "./tomasulo"
***********************************************************************
Support Operation Code

1. Add/Sub: integral and immediate.
2. Add.d/Sub.d: float and immediate.
3. Mul.d: float and immediate.
4. Sd/Ld: Example: Ld R1, 3(R1),
5. Beq/Bne: Example: Bne R1,R2,-3
***********************************************************************
