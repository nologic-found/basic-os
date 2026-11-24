/**
 *  A small Kernel with file read, write, delete, and program execution
 *
 * @author Kaitlyn, Cecilia, Bradley
 */

/* Special character constants */
#define CARRIAGE_RETURN   	'\r'
#define LINE_FEED         	'\n'
#define BACKSPACE_CHAR    	0x8
#define NULL_CHAR         	'\0'
#define SPACE_CHAR        	' '

/* Buffer and file system limits */
#define MAX_FILENAME      	6
#define MAX_CHAR_INPUT		80
#define SECTOR_SIZE       	512
#define MAX_SECTORS       	26
#define MAX_FILE_BUFFER   	13312
#define DIR_ENTRY_SIZE    	32
#define MAP_SECTOR        	1
#define DIR_SECTOR        	2
#define USER_SEGMENT      	0x2000

/* Kernel interrupt 0x21 service numbers */
#define SYS_PRINT_STRING  	0
#define SYS_READ_STRING   	1
#define SYS_READ_SECTOR   	2
#define SYS_READ_FILE     	3
#define SYS_EXEC_PROGRAM  	4
#define SYS_TERMINATE     	5
#define SYS_WRITE_SECTOR  	6
#define SYS_DELETE_FILE   	7
#define SYS_WRITE_FILE    	8

/* Declare functions */
void printString(char *);
void printChar(char);
void readString(char *);
void readSector(char *, int);
void writeSector(char *, int);
void readFile(char *, char *, int *);
void deleteFile(char *);
void writeFile(char *, char *, int);
void executeProgram(char *);
void terminate();
void handleInterrupt21(int, int, int, int);

/* test functions */
void testReadFile();

main()
{
	makeInterrupt21();
	interrupt(0x21, SYS_EXEC_PROGRAM, "shell", 0, 0);

	while (1)
	{
	}
}

/**
 * Prints a null-terminated character array to the screen at the current
 * cursor position using BIOS interrupt 0x10 (teletype output).
 *
 * Use case:
 * interrupt(0x21, SYS_PRINT_STRING, chars, 0, 0);
 * syscall(SYS_PRINT_STRING, chars, 0, 0);
 */
void printString(char *chars)
{
	int i = 0;
	while (chars[i] != NULL_CHAR)
	{
		char ah = 0xe;
		char al = chars[i];
		int ax = ah * 256 + al;

		interrupt(0x10, ax, 0, 0, 0);
		i++;
	}
}

/**
 * Prints a single ASCII character to the screen at the current cursor
 * position using BIOS interrupt 0x10 (teletype output).
 *
 * Use case:
 * printChar('x');
 */
void printChar(char c)
{
	interrupt(0x10, 0xE * 256 + c, 0, 0, 0);
}

/**
 * Reads keystrokes from the keyboard using BIOS interrupt 0x16 until
 * ENTER (CARRIAGE_RETURN) is pressed or MAX_INPUT characters are read.
 * Echoes typed characters, handles BACKSPACE_CHAR, and appends LINE_FEED
 * and NULL_CHAR.
 *
 * Use case:
 * interrupt(0x21, SYS_READ_STRING, line, 0, 0);
 * syscall(SYS_READ_STRING, line, 0, 0);
 */
void readString(char *line)
{
	char key_pressed;
	int i = 0;

	while (i < MAX_CHAR_INPUT)
	{
		key_pressed = interrupt(0x16, 0, 0, 0, 0);

		if (key_pressed != CARRIAGE_RETURN && key_pressed != BACKSPACE_CHAR)
		{
			printChar(key_pressed);
			line[i] = key_pressed;
			i++;
			continue;
		}
		if (key_pressed == BACKSPACE_CHAR)
		{
			if (i > 0)
			{
				i--;
				printChar(BACKSPACE_CHAR);
				printChar(SPACE_CHAR);
				printChar(BACKSPACE_CHAR);
			}
			continue;
		}
		if (key_pressed == CARRIAGE_RETURN)
		{
			line[i] = LINE_FEED;
			printChar(CARRIAGE_RETURN);
			printChar(LINE_FEED);
			i++;
			line[i] = NULL_CHAR;
		}
		break;
	}
}

/**
 * Reads a single 512-byte sector from the hard disk (drive 0x80) into
 * buffer using BIOS interrupt 0x13 (AH=2).
 *
 * Use case:
 * interrupt(0x21, SYS_READ_SECTOR, buffer, sector, 0);
 * syscall(SYS_READ_SECTOR, buffer, sector, 0);
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
 * Writes a single 512-byte sector from buffer to the hard disk (drive 0x80)
 * using BIOS interrupt 0x13 (AH=3).
 *
 * Use case:
 * interrupt(0x21, SYS_WRITE_SECTOR, buffer, sector, 0);
 * syscall(SYS_WRITE_SECTOR, buffer, sector, 0);
 */
void writeSector(char *buffer, int sector)
{
	int ah = 3;
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
 * Searches the disk directory (DIR_SECTOR) for a matching filename,
 * loads all sectors belonging to that file into buffer, and stores
 * the number of sectors read in *sectorsRead (0 if not found).
 *
 * Use case:
 * interrupt(0x21, SYS_READ_FILE, filename, buffer, &sectorsRead);
 * syscall(SYS_READ_FILE, filename, buffer, &sectorsRead);
 */
void readFile(char *filename, char *buffer, int *sectorsRead)
{
	char dir[SECTOR_SIZE];
	int fileentry = 0;
	int match = 0;
	int i = 0;
	int sectorOffset = 0;
	int currentSector = 0;

	*sectorsRead = 0;

	readSector(dir, DIR_SECTOR);

	for (fileentry = 0; fileentry < SECTOR_SIZE; fileentry += DIR_ENTRY_SIZE)
	{
		match = 1;

		if (dir[fileentry] == NULL_CHAR)
		{
			continue;
		}

		for (i = 0; i < MAX_FILENAME; i++)
		{
			if (filename[i] != dir[fileentry + i])
			{
				match = 0;
				break;
			}
			if (filename[i] == NULL_CHAR)
			{
				break;
			}
		}

		if (match)
		{
			i = 0;
			sectorOffset = fileentry + MAX_FILENAME;
			while (i < MAX_SECTORS && dir[sectorOffset] != 0)
			{
				currentSector = dir[sectorOffset] & 0xFF;
				readSector(buffer + SECTOR_SIZE * i, currentSector);
				i++;
				(*sectorsRead)++;
				sectorOffset++;
			}
			return;
		}
	}
}

/**
 * Deletes a file from the file system by marking its directory entry
 * deleted (first byte = NULL_CHAR) and freeing its sectors in the disk map.
 *
 * Use case:
 * interrupt(0x21, SYS_DELETE_FILE, filename, 0, 0);
 * syscall(SYS_DELETE_FILE, filename, 0, 0);
 */
void deleteFile(char *filename)
{
	char dir[SECTOR_SIZE];
	char map[SECTOR_SIZE];
	int fileentry = 0;
	int match = 0;
	int i = 0;
	int sectorOffset = 0;
	int sec = 0;

	readSector(map, MAP_SECTOR);
	readSector(dir, DIR_SECTOR);

	for (fileentry = 0; fileentry < SECTOR_SIZE; fileentry += DIR_ENTRY_SIZE)
	{
		if (dir[fileentry] == NULL_CHAR)
		{
			continue;
		}

		match = 1;
		/* compare filename against the entry's 6 bytes*/
		for (i = 0; i < MAX_FILENAME; i++)
		{
			if (filename[i] != dir[fileentry + i])
			{
				match = 0;
				break;
			}
			/* prevent reading past short filename like shell*/
			if (filename[i] == NULL_CHAR)
			{
				break;
			}
		}

		if (match)
		{
			/* mark as deleted*/
			dir[fileentry] = NULL_CHAR;

			/* free sector reserved by the file*/
			i = 0;
			sectorOffset = fileentry + MAX_FILENAME;
			while (i < MAX_SECTORS && dir[sectorOffset] != 0)
			{
				/* bitwise AND used to overcome signed char limitation:
				(-128 to 128) -> (0 to 255)
				*/
				sec = dir[sectorOffset] & 0xFF; 
				map[sec] = 0;
				i++;
				sectorOffset++;
			}

			/* save changes back to disk*/
			writeSector(map, MAP_SECTOR);
			writeSector(dir, DIR_SECTOR);
			return;
		}
	}
}

/**
 * Writes numberOfSectors sectors from buffer into a new file named filename,
 * allocating free sectors in the disk map (starting at sector 3) and
 * recording the file in a free directory entry.
 *
 * Use case:
 * interrupt(0x21, SYS_WRITE_FILE, buffer, filename, numberOfSectors);
 * syscall(SYS_WRITE_FILE, buffer, filename, numberOfSectors);
 */
void writeFile(char *buffer, char *filename, int numberOfSectors)
{
	char map[SECTOR_SIZE];
	char dir[SECTOR_SIZE];
	int fileentry = 0;
	int i = 0;
	int s = 0;
	int sec = 0;
	int found = 0;
	int sectorsToWrite = numberOfSectors;

	if (sectorsToWrite <= 0)
	{
		return;
	}
	if (sectorsToWrite > MAX_SECTORS)
	{
		sectorsToWrite = MAX_SECTORS;
	}

	readSector(map, MAP_SECTOR);
	readSector(dir, DIR_SECTOR);

	/* Find a free directory entry */
	found = 0;
	for (fileentry = 0; fileentry < SECTOR_SIZE; fileentry += DIR_ENTRY_SIZE)
	{
		if (dir[fileentry] == NULL_CHAR)
		{
			found = 1;
			break;
		}
	}
	if (!found)
	{
		return;
	}

	/* Copy filename (up to MAX_FILENAME chars) and pad remainder with NULL_CHAR */
	for (i = 0; i < MAX_FILENAME; i++)
	{
		dir[fileentry + i] = NULL_CHAR;
	}
	for (i = 0; i < MAX_FILENAME; i++)
	{
		if (filename[i] == NULL_CHAR)
		{
			break;
		}
		dir[fileentry + i] = filename[i];
	}

	/* Allocate and write each sector */
	for (s = 0; s < sectorsToWrite; s++)
	{
		found = 0;
		for (sec = 3; sec < 256; sec++)
		{
			if (map[sec] == 0)
			{
				found = 1;
				break;
			}
		}
		if (!found)
		{
			return;
		}

		map[sec] = 0xFF;
		dir[fileentry + MAX_FILENAME + s] = sec;
		writeSector(buffer + SECTOR_SIZE * s, sec);
	}

	/* Fill remaining sector slots in the directory entry with 0 */
	for (i = sectorsToWrite; i < MAX_SECTORS; i++)
	{
		dir[fileentry + MAX_FILENAME + i] = NULL_CHAR;
	}

	writeSector(map, MAP_SECTOR);
	writeSector(dir, DIR_SECTOR);
}

/**
 * Loads an executable file from disk, copies it to USER_SEGMENT,
 * and launches it. Prints an error if the file is not found.
 *
 * Use case:
 * interrupt(0x21, SYS_EXEC_PROGRAM, filename, 0, 0);
 * syscall(SYS_EXEC_PROGRAM, filename, 0, 0);
 */
void executeProgram(char *filename)
{
	int segment = USER_SEGMENT;
	char buffer[MAX_FILE_BUFFER];
	int sectorsRead = 0;
	int i = 0;

	readFile(filename, buffer, &sectorsRead);

	if (sectorsRead <= 0)
	{
		printString("File not found\r\n");
		return;
	}

	while (i < sectorsRead * SECTOR_SIZE)
	{
		putInMemory(segment, i, buffer[i]);
		i++;
	}
	launchProgram(segment);
}

/**
 * Terminates the current user program and reloads the shell via
 * interrupt 0x21.
 *
 * Use case:
 * interrupt(0x21, SYS_TERMINATE, 0, 0, 0);
 * syscall(SYS_TERMINATE, 0, 0, 0);
 */
void terminate()
{
	char shellname[MAX_FILENAME];
	shellname[0] = 's';
	shellname[1] = 'h';
	shellname[2] = 'e';
	shellname[3] = 'l';
	shellname[4] = 'l';
	shellname[5] = NULL_CHAR;

	interrupt(0x21, SYS_EXEC_PROGRAM, shellname, 0, 0);
}

/**
 * Debug/test helper that registers interrupt 0x21 and reads "messag".
 *
 * Use case:
 * testReadFile();
 */
void testReadFile()
{
	char buffer[MAX_FILE_BUFFER];
	int sectorsRead;
	makeInterrupt21();
	interrupt(0x21, SYS_READ_FILE, "messag", buffer, &sectorsRead);
	if (sectorsRead > 0)
		interrupt(0x21, SYS_PRINT_STRING, buffer, 0, 0);
	else
		interrupt(0x21, SYS_PRINT_STRING, "messag not found\r\n", 0, 0);
}

/**
 * Interrupt 0x21 dispatcher invoked by _interrupt21ServiceRoutine.
 *
 * Use case:
 * Invoked automatically on interrupt(0x21, ax, bx, cx, dx) or syscall(...).
 */
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	if (ax == SYS_PRINT_STRING)
	{
		printString((char *)bx);
	}
	else if (ax == SYS_READ_STRING)
	{
		readString((char *)bx);
	}
	else if (ax == SYS_READ_SECTOR)
	{
		readSector((char *)bx, cx);
	}
	else if (ax == SYS_READ_FILE)
	{
		readFile((char *)bx, (char *)cx, (int *)dx);
	}
	else if (ax == SYS_EXEC_PROGRAM)
	{
		executeProgram((char *)bx);
	}
	else if (ax == SYS_TERMINATE)
	{
		terminate();
	}
	else if (ax == SYS_WRITE_SECTOR)
	{
		writeSector((char *)bx, cx);
	}
	else if (ax == SYS_DELETE_FILE)
	{
		deleteFile((char *)bx);
	}
	else if (ax == SYS_WRITE_FILE)
	{
		writeFile((char *)bx, (char *)cx, dx);
	}
	else
	{
		printString("yeah idk what u did, so idk what to do");
	}
}
