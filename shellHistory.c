#include "shellHeader.h"

//function to add commands from user in terminal to history, calls invokeHistory
char* addToHistory(char *command, char *history[], int *count) { 

	//checks if command is a history invocation
	if (command[0] == '!') {
		return invokeHistory(history, command, *count); //returns the command invocated from history, will return "" on error
		//strcpy(command, (invokeHistory(history, command, *count)));
		if (strcmp(command, "") == 0) { //checks if invokeHistory returned an error
			return command;
		}
	}

	//if statement to check if the current size of history is less than max size
	if (*count < HISTORY_SIZE) {

		history[*count] = strdup(command); 	//sets latest element of history to command provided
		(*count)++; 						//increments history count, essentially updating size of history
		return history[(*count)-1]; 		//returns the command that was inputted by user (last thing added to history)

	} else { //if history size == max history size

		//loop to move all elements of history down one listing (i.e. invocation 0 is lost and invocation 1 takes its place)
		for (int i = 0; i < HISTORY_SIZE - 1; i++) { 
			free(history[i]); //frees current place in history
			history[i] = strdup(history[i + 1]); //sets freed place to command one increment ahead of it
		}
		
		history[HISTORY_SIZE - 1] = strdup(command); //sets final place in history to command provided by user (history size maxed no need to increment)
		
	} 

	return history[*count-1]; //returns the command that was inputted by user (last thing added to history)
}

//function to print every element held in history array
void showHistory(char* history[], int count) {
	//loop to print elements in array, stops when all elements have been traversed
	for (int i = 0; i < count; i++) {
		printf("%d: %s\n", i + 1, history[i]); //prints element of array with its coresponding spot in history
	}
}

//function to invoke histroy
char* invokeHistory(char* history[], char* command, int count) {
	
	//if history dose not have any items, don't invoke anything
	if (count == 0) {
		printf("%s: event not found: history has no contents\n", command); //error message
		return "";
	}

	//if user inputs '!!...'
	if (command[0] == '!' && command [1] == '!') { 
		
		//checks if anything was inputted after '!!' returns an error
		if (command[2] != '\0') {
			printf("%s: event not found: no arguements expected after '!!'\n", command);
			return "";
		} 

		return history[count-1]; //returns the most recent history invocation 

	} else if (command[0] == '!' && command [1] == '-') { //checks if the user is invocating a negative number

		//loop through every character and checks if there is any letters in the string, there should only be numbers
		for (int o = 2; o < strlen(command); o++) {
			if (isalpha(command[o])) { //checks if current character is a letter, returns error
				printf("%s: event not found: only numbers expected\n", command);
				return "";
			}
		}
		
		int sub_command = atoi(command+2); //changes everyting after '!-' to a number, will return 0 on invalid input (such as !-<no input>)

		//checks if the sub command <= 0, returns an error if so
		if (sub_command <= 0) {
			printf("%s: event not found: arguement must be a negative number\n", command);
			return "";
		} else if (count - sub_command < 0) { //checks if the current size of history - sub command < 0, returns error
			printf("%s: event not found: invocation too low -- (invocation: %d; not in history)\n", command, (count - sub_command + 1));
			return "";
		} else return history[(count)-sub_command]; //return the history invocation of the current history count subtracted by the sub command

	} else { //if the first character of input == '!'

		//loop through every character and checks if there is any letters in the string, there should only be numbers
		for (int o = 1; o < strlen(command); o++) {
			if (isalpha(command[o])) {
				printf("%s: event not found: only numbers expected\n", command);
				return "";
			}
		}

		int sub_command = atoi(command+1);
		
		if (count == 0) {
			printf("%s: event not found: history has no contents\n", command);
			return "";
		} else if (sub_command <= 0) {
			printf("%s: event not found: invocation must be a postive number\n", command);
			return "";
		} else if (sub_command > HISTORY_SIZE) {
			printf("%s: event not found: invocation outwidth history max size (current max size: %d)\n", command, HISTORY_SIZE);
			return "";
		} else if (sub_command > count) {
			printf("%s: event not found: invocation outwidth history contents (current size: %d)\n", command, count);
			return "";
		} else { 
			
			return history[sub_command-1];

		}

	}

}

//function to save all elements of history array to .hist_list file
void saveHistory(char* history[], int count) {

	setHomeDirectory(); //sets directory to user's home directory

	FILE *file; //decleration of file
	file = fopen(".hist_list", "w"); //opens .hist_list file in write mode

	//loop to write all contents of history to file, stops when all elements of history have been traversed
	for (int o = 0; o < count; o++) {
		fprintf(file, "%s\n", history[o]); //writes element of history to file
	}

	fclose(file); //closes .hist_list file

}

//function to load users history, fill history array with contents of .hist_list file
void loadHistory(char* history[], int* count) {

	char* file_name = ".hist_list"; 

	FILE *file; //decleration of file
	file = fopen(file_name, "r"); //opens .hist_list file in read mode

	//if statement to detect if file does not exist i.e. fopen returns NULL
	if (file == NULL) {
		return; //does not load anything into history array
	}

	char read_item[512]; //varaiable used to read lines from file

	//loop to add all lines from file to history, stops when it reaches end of file or there has been an error in reading
	while (!feof(file) && !ferror(file)) { 
		if (fgets(read_item, 512, file) != NULL) { //
			
			int size = strlen(read_item); //total size of string
			char* str = malloc(sizeof(char)*size); //creates a variable to copy read_item to
			strcpy(str, read_item); //copies content of read_item to str
			
			str[size-1] = '\0'; //removes \n from end of str
			history[*count] = strdup(str); //sets current history item to str
			(*count)++; //increments count
		}
	}
	
	fclose(file); //closes .hist_list file
	
}
