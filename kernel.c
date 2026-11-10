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
void readFile(char *, char *, int *);
main()
{
	char line[80];
	char buffer[512];

	int sectorsRead;
	char fileBuffer[13312]; // maximum file size
	makeInterrupt21();

	interrupt(0x21,3,"messag", fileBuffer, &sectorsRead);

	if(sectorsRead>0){
		interrupt(0x21, 0, fileBuffer, 0,0); // print the file
	}else
	{
		interrupt(0x21, 0, "message not found\r\n",0,0); // error report
	}

	interrupt(0x21,0,"\r",0,0); // stop being ugly
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
 * print a single character
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
 * Reads the sector at int sector and prints it into the buffer
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
 * @author Cecilia, Kaitlyn
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
		readFile((char *)bx, (char *)cx, (int *)dx);
	}
	else
	{
		printString("error: value of ax must be 0, 1, 2, or 3.");
	}

}


/**
 *@Author Cecilia, Kaitlyn
*/

void readFile(char *filename, char *buffer, int *sectorRead){
	// Init
	char dir[512];

        int fileentry; // used to track the entry
        int i; // used to check the 6 entires
        int bufferOffset; // used to avoid overwriting our data when we write to the fileBuffer
        int sectorsLoaded; // used to track how many sectors have already been proccessed

	*sectorRead = 0;

	// Peak inside sector 2
	readSector(dir,2);

	for(fileentry = 0; fileentry <512; fileentry += 32) // Each entry is 32 bytes
	{
		// Search the 6 entries for the right name
		for(i=0; i<6;i++)
		{

			if(filename[i] != dir[fileentry+i])
			{
				// Because we are breaking, sectorRead remains zero
				break;
			}
		}

		// if we didn't break, then the file name is correct
		if(i==6)
		{
			// We are in the first 512 bytes
			bufferOffset = 0;

			// There are only 26 sectors
			for(sectorsLoaded = 0; sectorsLoaded<26; sectorsLoaded++)
			{
				// check the sector in the directory, if it is zero then we have run out of sectors
				if(dir[fileentry+i+sectorsLoaded]==0)
				{
					break;
				}

				readSector(buffer + bufferOffset, dir[fileentry+6+sectorsLoaded]);

				// protect the data you just wrote
				bufferOffset += 512;
				(*sectorRead)++;
			}

			return;
		}
	}
}



