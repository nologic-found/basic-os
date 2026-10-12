
void printString(char *);
void printChar(char);
void readString(char *);

int main()
{

	char line[80];
	char *prompt = "Enter a line: ";
	printString(prompt);
	readString(line);
	printString(line);

	// Keeps kernel running as it should never stop
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

/**
 * Read keystroke and display it one screen
 *
 * Detail about Int 16/AH=00h
 * https://www.ctyme.com/intr/rb-1754.htm
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

	while (i<80)
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
				printChar(key_pressed);
				printChar(empty_space);
				printChar(key_pressed);
			}
			continue;
		}
		if (key_pressed == enter_key)
		{
			i++;
			printChar(line_feed);
			printChar(end_of_string);
			printChar(key_pressed);
			line[i] = line_feed;
			line[i+1] = end_of_string;
		}
		break;

	}
}