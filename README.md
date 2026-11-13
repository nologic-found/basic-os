# Cecilia Rosado, Kait Tucker, Bradley Chaurand
# Build an Operating System from Scratch Project


# ProjectB
Kait:
Developed the ReadSector

Brad:
Developed the readString

Cecilia:
developed the interruptHandler, printString, and printChar. Updated compile.sh accordingly.

What the code does: 

printString:
Prints a string/char array to the terminal 

printChar:
Prints a single character to the terminal 

readString:
reads the user inputs, allowing backspacing and enter key to "submit"

readSector: 
reads the whole sector in into the memory buffer so we cans search the memory buffer 

handleInterrupt21:
acts as a bridge to "initalize" or connect the interrupt function provided in the updated kernel.asm 

# Project C

Kait: developed readFile(), executeProgram(), terminate(), adjusted main() accordingly 

Cecilia: developed readFile(), shell.c, adjusted compile.sh accordingly.

What the code does:

shell.c:
provides the command line interface for the operating system. it accepts user commands, recognizes type and exec and uses system calls to communicate with the kernel. The type command prints the contents of the user-requested file and the exec command executes the user-requested program.

readFile():
searches disk directory for a requested filename and reads the files contents from disk into a buffer.

executeProgram():
loads a program from disk into memory at segment 0x2000 and uses launchProgram() (a Dr.Black provided assembly function) to execute it.

terminate():
ends the currently running program and reloads the shell so that the user can enter another command.

How to verify:
1. run 'bash compile.sh' in the project folder
2. in the simulator application, select Hard Drive/CD C and choose the file diskc.img. Set cylinders to 63, heads to 255 and sectors to 1000.
3. click Boot Disk C:
4. After boot, shell prompt will show. To verify that its working at this stage, enter 'type messag' and press enter which will print out the contents of message.txt and then will return to the shell prompt. Then, type 'exec tstpr2' and press enter which will execute the program and then will return back to the shell prompt.
