# Cecilia Rosado, Kait Tucker, Bradley Chaurand
# DOS like os project

The final result should be something like a primitive CLI dos OS.

Kait:
Developed the ReadSector

Brad:
Developed the readString


Cecilia:
developed the interruptHandler, printString, and printChar. Rebuilt main to use the interrupt function, Fixed kernel.asm, Created and 
inserted message.txt into the sector. Updated complileOS.

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
