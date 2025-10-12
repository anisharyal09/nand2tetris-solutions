// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

//* Description from Nand2Tetris project materials.  

//Initialize: R2 = 0
 @R2
 M=A // R2 = 0

 // Check for R0 == 0
 @R0
 D=M
 @R2
 M=0    // R2 = 0
 @END
 D;JEQ

// Check for R1==0
 @R1
 D=M
 @R2
 M=0    // R2 = 0
 @END
 D;JEQ

 //Loop if R1 != 0 || R0 != 0
 //Adding R0 repeatedly R1 times, decrementing R1 each time until R1==0
 (LOOP)

 @R0
 D=M
 @R2
 M=M+D // R2 = R2 + R0

 // Decrementing R1
 @R1
 M=M-1
 D=M

 @LOOP
 D;JGT // If R1 > 0, continue the loop
 
 (END)
 @END
 0;JMP