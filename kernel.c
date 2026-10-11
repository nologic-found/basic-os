
void printString(char *);
void printChar(char);

int main()
{

	char *msg = "Hi there ";
	// int offset = 0;
	// while(msg[lettercount] != '\0'){
	// 	int offset = lettercount*2;
	// 	putInMemory(0xb800,offset,msg[lettercount]);
	// 	putInMemory(0xb800,offset+1,color);
	// 	lettercount++;
	// }

	printString(msg);
	while (1)
	{
	}
}

/**
 * Prints character array to screen
 * 
 * Detail about Int 10/AH=0Eh
 * https://www.ctyme.com/intr/rb-0106.htm	
 */
void printString(char *chars)
{
	int i = 0;
	while (chars[i] != '\0')
	{
		char ah = 0xe;
		char al = chars[i];
		int ax = ah * 256 + al;

		// prints one character at the current cursor position
		interrupt(0x10, ax, 0, 0, 0);
		i++;
	}
}

/**
 * Prints one character to screen
 * 
 * Detail about Int 10/AH=0Eh
 * https://www.ctyme.com/intr/rb-0106.htm	
 */
void printChar(char c)
{
	char ah = 0xe;
	char al = c;
	int ax = ah * 256 + al;

	// prints one character at the current cursor position
	interrupt(0x10, ax, 0, 0, 0);
}
