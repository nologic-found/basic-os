/**
 *  A very small Kernel that prints and reads from user input
 *
 * @author Kaitlyn, Cecilia, Bradley
 */
// Declare functions
void printString(char *);
void printChar(char);
void readString(char *);
void readSector(char *, int);
void handleInterrupt21(int, int, int, int);
void readFile(char *);
main()
{
	char line[80];
	char buffer[512];

	int sectorsRead;
	char fileBuffer[13312];	
	makeInterrupt21();
	interrupt(0x21, 3, "messag", fileBuffer, &sectorsRead); /*read the file into buffer*/
		if(sectorsRead>0)
			interrupt(0x21, 0, fileBuffer, 0,0); /*print out the file*/
		else
			interrupt(0x21, 0, "message not found\r\n",0,0); /*print  an error if no sectors are read*/
	//	testing read sector from my teammates code

	interrupt(0x21, 2, buffer, 30, 0); // Read sector 30 where message.txt should be

	buffer[511] = 0; // Ensure printString stops before crashing out of buffer
	buffer[78] = 0;	 // Fixed extra newlining?

	interrupt(0x21, 0, buffer, 0, 0); // Print Sector content
	interrupt(0x21, 0, "\n\r", 0, 0); // Stop looking like bricked trash

      interrupt(0x21,0, "Is interrupt 21 working? \r\n", 0,0);

	interrupt(0x21, 1, line, 0, 0);
	interrupt(0x21, 0, line, 0, 0);
	
	while(1){}
	return 0;
}

/**
 * Prints character array to screen
 *
 * Detail about Int 10/AH=0Eh
 * https://www.ctyme.com/intr/rb-0106.htm
 *
 * @author Bradley Chaurand
 */
void printString(char *chars)
{
	char end_of_string = 0x0;
	int i = 0;
	while (chars[i] != end_of_string)
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
 * @author Kaitlyn
 */
void printChar(char c)
{
	interrupt(0x10, 0xE * 256 + c, 0, 0, 0);
}

/**
 * Read keystroke and display it one screen
 *
 * Detail about Int 16/AH=00h
 * https://www.ctyme.com/intr/rb-1754.htm
 * @author Bradley Chaurand
 */
void readString(char *line)
{
	char key_pressed;
	char enter_key = 0xd;
	char backspace = 0x8;
	char line_feed = 0xa;
	char end_of_string = 0x0;
	char empty_space = 0x20;
	int i = 0;

	while (i < 80)
	{
		char key_pressed = interrupt(0x16, 0, 0, 0, 0);

		if (key_pressed != enter_key && key_pressed != backspace)
		{
			printChar(key_pressed);
			line[i] = key_pressed;
			i++;
			continue;
		}
		if (key_pressed == backspace)
		{
			if (i > 0)
			{
				i--;
				printChar(backspace);//printChar(key_pressed);
				printChar(empty_space);
				printChar(backspace);//printChar(key_pressed);
			}
			continue;
		}
		if (key_pressed == enter_key)
		{
			line[i] = line_feed;//i++; 
			printChar(line_feed);
			i++;
			line[i] = end_of_string;//printChar(end_of_string);
			printChar(key_pressed);
			//line[i] = line_feed;
			//line[i + 1] = end_of_string;
		}
		break;
	}
}

/**
 * @author Kaityln
 */
void readSector(char *buffer, int sector)
{
	int ah = 2;
	int al = 1;
	int ch = 0;
	int cl = sector + 1;
	int dh = 0;
	int dl = 0x80;
	int ax = ah * 256 + al;
	int cx = ch * 256 + cl;
	int dx = dh * 256 + dl;
	interrupt(0x13, ax, buffer, cx, dx);
}

/**
 * @author Cecilia
 */
void handleInterrupt21(int ax, int bx, int cx, int dx)
{

	if (ax == 0)
	{
		printString((char *)bx);
		//		printString(bx);
	}
	else if (ax == 1)
	{
		readString((char *)bx);
		//		readString(bx);
	}
	else if (ax == 2)
	{
		readSector((char *)bx, cx);
		//		readSector(bx, cx);
	}
	else if (ax == 3){
		readFile((char *)bx, cx, dx);
	}
	else
	{
		printString("error: value of ax must be 0, 1, 2, or 3.");
	}
	
	break;
}




void readFile(char* filename){
	char* dir;

	dir = interrupt(2, buffer, 2); 
	for(int fileentry = 0; fileentry < 512; fileentry +=32){ //loads the directory sector into 512 char array
		 for(int i = 0; i < 7, i++){ //looks for filename
			if (filename[i]==dir[fileentry+i]){ //if first byte is also the first byte of the filename do nothing
				continue;
			}
			else if (i = 6){ // if we made it to the 7th byte (first byte after filename)
				for(int sectorsRead = 0; int sectorsRead < 26; sectorsRead++){
					int i = 1;
					interrupt(2, buffer + 512*i, dir[fileentry + sectorsRead++);
					i++;
					if (fileentry[sectorsRead] == 0){
						sectorsRead = 0;
						break;
					}
				}
			}
			else {
				break;

			}
		} 
	}
}
