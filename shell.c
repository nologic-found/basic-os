/* @author Cecilia, Bradley */

/* Special character constants */
#define CARRIAGE_RETURN		'\r'
#define LINE_FEED         	'\n'
#define NULL_CHAR         	'\0'
#define SPACE_CHAR        	' '
#define ZERO_CHAR         	'0'

/* Buffer and file system limits */
#define MAX_FILENAME      	6
#define MAX_CHAR_INPUT		80
#define SECTOR_SIZE       	512
#define MAX_SECTORS       	26
#define MAX_BUFFER        	13313

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

int syscall();

/* System call wrappers */
void sysPrint(char *);
void sysReadLine(char *);
void sysReadSector(char *, int);
void sysReadFile(char *, char *, int *);
void sysExec(char *);
void sysDeleteFile(char *);
void sysWriteFile(char *, char *, int);

/* Helper functions */
void clearArray(char *, int);
void stripNewline(char *, int);
int startsWith(char *, char *);
int stringEquals(char *, char *);
int extractWord(char *, int, char *, int);
void printInt(int);

/* Shell command functions */
void cmdDir(char *);
void cmdType(char *, char *);
void cmdExec(char *, char *);
void cmdDel(char *);
void cmdCopy(char *, char *, char *);
void cmdCreate(char *, char *);

int main()
{
	char buffer[MAX_BUFFER];
	char filename[MAX_FILENAME + 1];
	char filename2[MAX_FILENAME + 1];
	char user_input[MAX_CHAR_INPUT];
	int nextIdx = 0;

	while (1)
	{
		clearArray(filename, MAX_FILENAME + 1);
		clearArray(filename2, MAX_FILENAME + 1);
		clearArray(user_input, MAX_CHAR_INPUT);

		sysPrint("SHELL:>");
		sysReadLine(user_input);

		stripNewline(user_input, MAX_CHAR_INPUT);

		if (user_input[0] == NULL_CHAR)
		{
			continue;
		}

		if (stringEquals(user_input, "dir"))
		{
			cmdDir(buffer);
		}
		else if (startsWith(user_input, "type "))
		{
			extractWord(user_input, 5, filename, MAX_FILENAME);
			cmdType(filename, buffer);
		}
		else if (startsWith(user_input, "exec "))
		{
			extractWord(user_input, 5, filename, MAX_FILENAME);
			cmdExec(filename, buffer);
		}
		else if (startsWith(user_input, "del "))
		{
			extractWord(user_input, 4, filename, MAX_FILENAME);
			cmdDel(filename);
		}
		else if (startsWith(user_input, "copy "))
		{
			nextIdx = extractWord(user_input, 5, filename, MAX_FILENAME);
			extractWord(user_input, nextIdx, filename2, MAX_FILENAME);
			cmdCopy(filename, filename2, buffer);
		}
		else if (startsWith(user_input, "create "))
		{
			extractWord(user_input, 7, filename, MAX_FILENAME);
			cmdCreate(filename, buffer);
		}
		else
		{
			sysPrint("Command not found.\r\n");
		}
	}

	return 0;
}

/**
 * Prints a string to the screen.
 */
void sysPrint(char *str)
{
	syscall(SYS_PRINT_STRING, str, 0, 0);
}

/**
 * Reads a line of keyboard input into buffer.
 */
void sysReadLine(char *buffer)
{
	syscall(SYS_READ_STRING, buffer, 0, 0);
}

/**
 * Reads one 512-byte sector from disk into buffer.
 */
void sysReadSector(char *buffer, int sector)
{
	syscall(SYS_READ_SECTOR, buffer, sector, 0);
}

/**
 * Reads a file from disk into buffer and stores sector count in *sectorsRead.
 */
void sysReadFile(char *name, char *buffer, int *sectorsRead)
{
	syscall(SYS_READ_FILE, name, buffer, sectorsRead);
}

/**
 * Loads and executes a program by filename.
 */
void sysExec(char *name)
{
	syscall(SYS_EXEC_PROGRAM, name, 0, 0);
}

/**
 * Deletes a file by filename.
 */
void sysDeleteFile(char *name)
{
	syscall(SYS_DELETE_FILE, name, 0, 0);
}

/**
 * Writes sectors from buffer to a new file named name.
 */
void sysWriteFile(char *buffer, char *name, int sectors)
{
	syscall(SYS_WRITE_FILE, buffer, name, sectors);
}

/**
 * Lists all non-deleted files in the directory and their sizes.
 */
void cmdDir(char *buffer)
{
	char filename[MAX_FILENAME + 1];
	int i = 0;	/* directory index: 0, 32, 64 ...*/
	int j = 0;	/* filename index:0...6*/
	int secCount = 0;

	/* directory is at sector 2*/
	sysReadSector(buffer, 2);

	/* loop through each entry of directory*/
	for (i = 0; i < SECTOR_SIZE; i += 32)
	{
		/* if first entry char of that entyr is empty, skip it*/
		if (buffer[i] == NULL_CHAR)
		{
			continue;
		}

		/* save filename*/
		for (j = 0; j < MAX_FILENAME; j++)
		{
			filename[j] = buffer[i + j];
		}
		filename[MAX_FILENAME] = NULL_CHAR;

		secCount = 0;
		while (secCount < MAX_SECTORS && buffer[i + MAX_FILENAME + secCount] != 0)
		{
			secCount++;
		}

		sysPrint(filename);
		sysPrint(" (");
		printInt(secCount);
		sysPrint(" sectors)\r\n");

		/* print bytes with sectors * SECTOR_SIZE*/
	}
}

/**
 * Reads and prints the contents of a file.
 */
void cmdType(char *filename, char *buffer)
{
	int sectorsRead = 0;

	/* empty filename*/
	if (filename[0] == NULL_CHAR)
	{
		sysPrint("File not found.\r\n");
		return;
	}

	sysReadFile(filename, buffer, &sectorsRead);
	if (sectorsRead > 0)
	{
		/* make sure to terminate the file properly in case it was not*/
		buffer[sectorsRead * SECTOR_SIZE] = NULL_CHAR;
		sysPrint(buffer);
		sysPrint("\r\n"); 
	}
	else
	{
		sysPrint("File not found.\r\n");
	}
}

/**
 * Verifies a program exists and executes it.
 */
void cmdExec(char *filename, char *buffer)
{
	int sectorsRead = 0;

	if (filename[0] == NULL_CHAR)
	{
		sysPrint("File not found.\r\n");
		return;
	}

	sysReadFile(filename, buffer, &sectorsRead);
	if (sectorsRead > 0)
	{
		sysExec(filename);
	}
	else
	{
		sysPrint("File not found.\r\n");
	}
}

/**
 * Deletes a file from the disk.
 */
void cmdDel(char *filename)
{
	if (filename[0] == NULL_CHAR)
	{
		sysPrint("File not found.\r\n");
		return;
	}

	sysDeleteFile(filename);
}

/**
 * Copies filename1 to filename2 using file read and write system calls.
 */
void cmdCopy(char *filename1, char *filename2, char *buffer)
{
	int sectorsRead = 0;

	if (filename1[0] == NULL_CHAR || filename2[0] == NULL_CHAR)
	{
		sysPrint("Usage: copy <filename1> <filename2>\r\n");
		return;
	}

	sysReadFile(filename1, buffer, &sectorsRead);
	if (sectorsRead > 0)
	{
		sysWriteFile(buffer, filename2, sectorsRead);
	}
	else
	{
		sysPrint("File not found.\r\n");
	}
}

/**
 * Prompts line-by-line until an empty line and writes a new text file.
 * Save by pressing enter on empty line
 */
void cmdCreate(char *filename, char *buffer)
{
	char line[MAX_CHAR_INPUT];
	int i = 0;
	int j = 0;
	int bufferLength = 0;

	/* between 1 and 26 max*/
	int sectors = 0;

	/* help message if user type "create" without a filename*/
	if (filename[0] == NULL_CHAR)
	{
		sysPrint("Usage: create <filename>\r\n");
		return;
	}

	bufferLength = 0;
	while (1)
	{
		clearArray(line, MAX_CHAR_INPUT);

		sysPrint("> ");
		sysReadLine(line);

		/* check for return*/
		if (line[0] == LINE_FEED || line[0] == CARRIAGE_RETURN || line[0] == NULL_CHAR)
		{
			break;
		}

		j = 0;
		while (line[j] != NULL_CHAR && bufferLength < MAX_BUFFER - 2)
		{
			if (line[j] == LINE_FEED && bufferLength < MAX_BUFFER - 3)
			{
				buffer[bufferLength++] = CARRIAGE_RETURN;
			}
			buffer[bufferLength++] = line[j++];
		}
	}

	sectors = 1;
	i = bufferLength;
	while (i >= SECTOR_SIZE)
	{
		i = i - SECTOR_SIZE;
		sectors++;
	}
	if (sectors > MAX_SECTORS)
	{
		sectors = MAX_SECTORS;
	}

	for (i = bufferLength; i < sectors * SECTOR_SIZE; i++)
	{
		buffer[i] = NULL_CHAR;
	}

	sysWriteFile(buffer, filename, sectors);
}

/**
 * Fills a character array with NULL_CHAR.
 */
void clearArray(char *arr, int size)
{
	int i = 0;
	while (i < size)
	{
		arr[i] = NULL_CHAR;
		i++;
	}
}

/**
 * Replaces the first LINE_FEED or CARRIAGE_RETURN in str with NULL_CHAR.
 */
void stripNewline(char *str, int maxLen)
{
	int i = 0;
	while (i < maxLen && str[i] != NULL_CHAR)
	{
		if (str[i] == LINE_FEED || str[i] == CARRIAGE_RETURN)
		{
			str[i] = NULL_CHAR;
			break;
		}
		i++;
	}
}

/**
 * Returns 1 if str starts with prefix, 0 otherwise.
 */
int startsWith(char *str, char *prefix)
{
	int i = 0;
	while (prefix[i] != NULL_CHAR)
	{
		if (str[i] != prefix[i])
		{
			return 0;
		}
		i++;
	}
	return 1;
}

/**
 * Returns 1 if two strings are equal, 0 otherwise.
 */
int stringEquals(char *s1, char *s2)
{
	int i = 0;
	while (s1[i] != NULL_CHAR && s2[i] != NULL_CHAR)
	{
		if (s1[i] != s2[i])
		{
			return 0;
		}
		i++;
	}
	return s1[i] == s2[i];
}

/**
 * Copies up to maxLen characters of the next space-delimited word from src
 * starting at idx into dest, and returns the index right after that word.
 */
int extractWord(char *src, int idx, char *dest, int maxLen)
{
	int count = 0;

	while (src[idx] == SPACE_CHAR)
	{
		idx++;
	}

	while (count < maxLen && src[idx] != NULL_CHAR && src[idx] != SPACE_CHAR)
	{
		dest[count] = src[idx];
		count++;
		idx++;
	}
	dest[count] = NULL_CHAR;

	while (src[idx] != NULL_CHAR && src[idx] != SPACE_CHAR)
	{
		idx++;
	}

	return idx;
}

/**
 * Prints a small non-negative integer without / or %.
 */
void printInt(int n)
{
	char out[6];
	int i = 0;
	int tens = 0;
	int hundreds = 0;

	if (n <= 0)
	{
		sysPrint("0");
		return;
	}

	while (n >= 100)
	{
		n = n - 100;
		hundreds++;
	}
	while (n >= 10)
	{
		n = n - 10;
		tens++;
	}

	if (hundreds > 0)
	{
		out[i++] = ZERO_CHAR + hundreds;
	}
	if (hundreds > 0 || tens > 0)
	{
		out[i++] = ZERO_CHAR + tens;
	}
	out[i++] = ZERO_CHAR + n;
	out[i] = NULL_CHAR;
	sysPrint(out);
}
