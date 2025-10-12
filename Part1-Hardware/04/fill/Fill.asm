// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//* Description from Nand2Tetris project materials.  

(OUT_LOOP)

@SCREEN
D=A 
@scr
M=D   //initialize scr to SCREEN
@i
M=0     //initialize i to 0

//Keyboard detection
@KBD
D=M

//if D==0, key not pressed - jumps to whiten screen
@WHITE_LOOP
D;JEQ

//else - blacken screen
(BLACK_LOOP)
@scr
A=M
M=-1
@scr
M=M+1   //scr=scr+1 - next 16pxs

@i
M=M+1   // i=i+1
D=M
@8192  //total size of screen in 16pxs
D=D-A
@BLACK_LOOP
D;JLT   //if i<8192, continue BLACK_LOOP

@OUT_LOOP
0;JMP   //Listen continuously to keyboard input


//whiten screen
(WHITE_LOOP)
@scr
A=M
M=0

@scr
M=M+1

@i
M=M+1   // i=i+1
D=M
@8192 
D=D-A
@WHITE_LOOP
D;JLT   //if i<8192, continue WHITE_LOOP

@OUT_LOOP
0;JMP

//
// My implementation: Uses separate BLACK_LOOP and WHITE_LOOP
// Alternative approach found: uses single DRAW loop with fillvalue
// Credit: https://github.com/davidsmithmke/nand2tetris-project4