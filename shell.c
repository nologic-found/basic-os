/* @author Cecilia */
int syscall();

int main(){
	char buffer[13312]; 	//stores file contents
	char filename[7]; 	//stores 6 char filename and a null terminator
	char user_input[100];   //stores command entered by user
	int i = 0;    		//loop counter for getting the filename from user_input
	int j = 0;		//loop counter for clearing arrays
	int sectorsRead;	//tracks how many sectors were read
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
		
		syscall(6, 13);  	//set cursor position to beginning of line
		syscall(0, "SHELL:>"); 	//print shell prompt
		syscall(1, user_input); //reads the command entered by user
		

		//checks if the user entered 'type' command
		if(user_input[0] == 't' && user_input[1] == 'y' && 
		user_input[2] == 'p' && user_input[3] == 'e' && user_input[4] == ' ') {
			
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
		user_input[2] == 'e' && user_input[3] == 'c' && user_input[4] == ' ')   {
		
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
		}	

		//runs if command is not recognized by the shell
	else {
		syscall(0, "Command not found. \r\n");
		}
		
	}
 return 0;	
}
