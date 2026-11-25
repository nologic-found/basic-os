/* @author Cecilia */
int syscall();
void dir();
int mod(int, int);

int main(){
	char buffer[13312]; 	//stores file contents
	char filename[7]; 	//stores 6 char filename and a null terminator
	char user_input[100];   //stores command entered by user
	int i = 0;    		//loop counter for getting the filename from user_input
	int j = 0;		//loop counter for clearing arrays
	int sectorsRead;	//tracks how many sectors were read

	char test[2]; 		// Testing if this works
	while(1){

		j = 0;
		while(j < 7){
			filename[j] = '\0'; 	//clear filename array before each new command
			j++;
		}

		j = 0;
		while(j < 100){
			user_input[j] = '\0'; 	//clear user input array before each new command
			j++;
		}

		sectorsRead = 0; 	//reset sectorsRead
		test[0] = 13;		// fixing prints
		test[1] = '\0';

		syscall(0, test);  	//set cursor position to beginning of line

		syscall(0, "SHELL:>"); 	//print shell prompt
		syscall(1, user_input); //reads the command entered by user


		//checks if the user entered 'type' command
		if(user_input[0] == 't' && user_input[1] == 'y' &&
		user_input[2] == 'p' && user_input[3] == 'e' && (user_input[4] == ' '|| user_input[4] == '\0')) {

			i = 0;		//reset counter

			// copy up to 6 chars of the filename from user input
			while(i < 6 && user_input[i+5] != '\0'){
				filename[i] = user_input[i+5];
				i++;
			}

			filename[i] = '\0';  //add null terminator to filename

			// make sure filename contains exacly 6 chars
			if(user_input[11] != '\0' || user_input[10] == '\0'){
				syscall(0, "File not found. Filename must be the first 6 characters of the file. \r\n");
			}

			else{
				//read the file that user entered into buffer
				syscall(3, filename, buffer, &sectorsRead);

				// checks if the file was successfully read, if so than display its contents
				if(sectorsRead > 0){
					syscall(0, buffer);
				}

				// error if file wasn't found
				else {
			  		syscall(0, "File not found.\r\n");
				}
			}
		}

		//checks whether user entered "exec" command
		else if(user_input[0] == 'e' && user_input[1] == 'x' &&
		user_input[2] == 'e' && user_input[3] == 'c' && (user_input[4] == ' ' || user_input[4] == '\0' )){

			i = 0; 	//reset counter

			// copy up to 6 chars of filename from user input
			while(i < 6 && user_input[i+5] != '\0'){
				filename[i] = user_input[i+5];
				i++;
			}

			filename[i] = '\0'; 	//add null terminator to filename

			//make sure filename contains exactly 6 chars
			if(user_input[11] != '\0' || user_input[10] == '\0'){
				syscall(0, "File not found. Filename must be the first 6 characters of the file. \r\n");
			}

			else{
				//read the program  that user entered into buffer
				syscall(3, filename, buffer, &sectorsRead);

				// checks if program was found, if so then executes program
				if(sectorsRead > 0){
					syscall(4, filename);
				}

				// error if program not found
				else {
			  		syscall(0, "File not found.\r\n");
				}
			}

		}else if (user_input[0] == 'd' && user_input[1] == 'i' && user_input[2] == 'r' && (user_input[4] == ' ' || user_input[4] == '\0')){
			dir();
		}

		//runs if command is not recognized by the shell
	else {

		syscall(0, "\r\n Command not found. \r\n");
		}

	}
 return 0;
}

void dir(){
	char buffer[512]; // create the buffer
	char files[512]; // create the file names buffer
	char numBuff[10]; // temp buffer to hold digits

	int i = 0; // loop var
	int j; // nested loop var#1
	int k; // nested loop var#2
	int z; // nested loop var#3

	int filesCount = 0; // count for the inserting the file names
	int sectorsCount = 0; // count the sectors after the name
	int fileSize;	// track the size of the file
	int numDigits;	// track how many digits are in the file to display one character at a time l

	// do read sector, read into buffer, read sector2
	syscall(2, buffer, 2);

	// Read through the whole sector to create the file names
	for(i = 0; i<512; i+=32){
		// add an indent
		for(k = 0; k < 5 ; k++){
			files[filesCount] = ' ';
			filesCount++;
		}

		// read the filename into the files buffer
		for(j = 0; j < 6; j++){

			// if its a null character (LOOKING AT YOU SHELL) then skip to end
			if(buffer[i+j] == '\0'){
				// if its also the first character then just skip everything
				if(j == 0){
					j = 6;
				}else{
					j = 5;
				}
			}

			// if its the end of the filename then add a newline
			if(j == 5){

				// if there is a character then add the last character
				if(buffer[i+j] != '\0'){
					files[filesCount] = buffer[i+j];
					filesCount++;
				}

				// attach 'size: '
				files[filesCount] = ' ';
				files[filesCount+1] = 's';
				files[filesCount+2] = 'i';
				files[filesCount+3] = 'z';
				files[filesCount+4] = 'e';
				files[filesCount+5] = ':';
				files[filesCount+6] = ' ';
				filesCount+=7;


				// reset sectorCount
				sectorsCount = 0;
				for(z = 6; z<32; z++){
					if(buffer[i+z] == '\0'){
						break;
					}
					sectorsCount++;
				}

				// find the file size
				fileSize = sectorsCount * 512;
				// reset numDigits
				numDigits = 0;

				// default case but lwk should never happen
				if(fileSize == 0){
					files[filesCount] = '0';
					filesCount++;

				// start extracting numbers
				}else{
					while(fileSize > 0){
						// take the last digit of the fileSize and turn it into printable ASCII until we have every digit
						numBuff[numDigits] = mod(fileSize, 10) + '0';
						fileSize = fileSize / 10;
						numDigits++;
					}

					// write into files
					while(numDigits > 0){
						numDigits--; // we start beyond the loop so we have to step back into the right place
						files[filesCount] = numBuff[numDigits];
						filesCount++;
					}
				}

				// add the word ' bytes'
				files[filesCount] = ' ';
				files[filesCount+1] = 'b';
				files[filesCount+2] = 'y';
				files[filesCount+3] = 't';
				files[filesCount+4] = 'e';
				files[filesCount+5] = 's';
				filesCount+=6;

				// now add newline
				files[filesCount] = '\n';
				files[filesCount+1] = '\r';
				filesCount+=2;


			}else if(j != 6){

				// should be normal part of a name here
				files[filesCount] = buffer[i+j];
				filesCount++;
			}

		}
	}

	// end any printing from here on out
	files[filesCount] = '\0';

	// print the file names
	syscall(0, files);

}

int mod(int a, int b){
	return a - (a/b) * b;
}
