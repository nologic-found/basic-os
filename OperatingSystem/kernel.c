// Kait
// A very small Kernel that prints and reads from user input

// Declare my functions
void printString(char*);
void printChar(char);
void readString(char*);
void readSector(char*, int);
void handleInterrupt21(int,int,int,int);

main()
{
	char line[80];
	char buffer[512];


	makeInterrupt21();

//	testing read sector from my teammates code

	interrupt(0x21, 2, buffer, 30, 0); // Read sector 30 where message.txt should be 

	buffer[511]=0; // Ensure printString stops before crashing out of buffer 
	buffer[78]=0; // Fixed extra newlining?

	interrupt(0x21, 0, buffer, 0, 0); // Print Sector content
	interrupt(0x21, 0, "\n\r", 0, 0); // Stop looking like bricked trash

//      interrupt(0x21,0, "Is interrupt 21 working? \r\n", 0,0);

        interrupt(0x21, 1, line, 0,0);
        interrupt(0x21, 0, line, 0, 0);


	while(1){}
	return 0;
}



void printString(char* chars)
{
	int i;
	char al;
	char ah;
	int ax;

	for( i = 0; chars[i] != '\0'; i++)
	{
		printChar(chars[i]);

//		This is how I did it originally
//		al = chars[i];
//		ah = 0xE;
//		ax = (ah*256)+al;
//		interrupt(0x10, ax, 0,0,0);
	}
}

void printChar(char c)
{

	interrupt(0x10, 0xE*256 + c, 0,0,0);
}

void readString(char* line)
{
	int i = 0;
	char c;
	while(1)
	{
		c = interrupt(0x16, 0,0,0,0);
		if(c==0x0D)// Enter
		{
			printChar(0x0D);
			printChar(0x0A);
			line[i] = 0x0A;
			i++;
			line[i] = 0x0D;
			i++;
			line[i] = 0x00;

//			line[i] = 0x0A;
//			printChar(line[i]); // print line feed
//			i++;
//			line[i] = 0x00; // Add null terminator
			return;
		}
		else if(c==0x8) // Backspace
		{
			if(i>0)
			{
				i--; // Move back in array
				printChar(0x08); // Move curser back
				printChar(0x20); // space to remove char
				printChar(0x08);
			}
		}else
		{
			if(i<78)
			{
				line[i] = c;
				printChar(line[i]);
				i++;
			}

		}
	}
}

void readSector(char* buffer, int sector){
        int ah = 2;
        int al = 1;
        int ch = 0;
        int cl = sector + 1; 
        int dh = 0;
        int dl = 0x80;
        int ax = ah*256+al;
        int cx = ch*256+cl;
        int dx = dh*256+dl; 
        interrupt(0x13, ax, buffer, cx, dx);
}



 void handleInterrupt21(int ax, int bx, int cx, int dx)
{

	if(ax == 0)
	{
		printString((char*)bx);
//		printString(bx);
	} else if(ax == 1)
	{
		readString((char*)bx);
//		readString(bx);
	} else if(ax == 2)
	{
		readSector((char*)bx, cx);
//		readSector(bx, cx);
	} else
	{
		printString("yeah idk what u did, so idk what to do");
	}
}

