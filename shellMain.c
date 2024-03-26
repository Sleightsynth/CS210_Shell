#include "shellHeader.h"

int main(void) {
	
	char input[512]; 		//input from user
	char *check_input = ""; 	//used in comparsions to check user input
	char *path = getUserPath();	//saves original path from user
	char *history[HISTORY_SIZE];
	int historyCount = 0;

	aliasEntry aliasList[10];
	int aliases = 0;
	
	setHomeDirectory(); 					//sets directory to users home directory
	loadHistory(history, &historyCount); 	//loads users history from .hist_list
	loadAlias(aliasList, &aliases); 		//loads aliases from .aliases

	//infinte loop, terminates when conditional is tripped
	while (1) {
		
		printPrompt(); //prints prompt
		check_input = fgets(input, sizeof(input), stdin); //(fgets) gets input from user, (check_input) used to check if the input is NULL i.e. command+D
		
		//conditional checking if command+D or "exit" has been inputted
		if (check_input == NULL || strcmp(check_input, "exit\n" ) == 0) {
			break; //terminates loop
		} else if (strcmp(check_input, "\n") == 0) { //condtional to return to start of loop if only enter is inputted
			continue; //returns to begining of while loop
		} else if (strcmp(check_input, " \n") == 0 || strcmp(check_input, "\t\n") == 0) {
			continue;
		}

		check_input[strlen(check_input)-1] = '\0'; //takes out the \n (newline) char from end of string

		if (getAliasCommand(check_input, aliasList, aliases) != '!') {
			strcpy(check_input, addToHistory(check_input, history, &historyCount));
		}
		
		//if statement that will trigger if a history invocation/adding to history fails
		if (strcmp(check_input, "") == 0) {
			continue;
		} 
		
		//while loop that updates inputted alias to its coresponding command, will keep looping until final non-alias command is found
		while (isAlias(check_input, aliasList, 1, aliases) == 1) {
			if (updateAlias(check_input,aliasList,aliases) == 0) {
                perror("Alias Error.");
            }
		}
		
        if (check_input[0] == '!') {
        	strcpy(check_input, invokeHistory(history, check_input, historyCount));
        }

		char* delim = " \t\n;&><|";					//new variable to be used as a delimiter
		char* token = strtok(check_input, delim);	//new variable to store the first token
		char* tokenArray[256]; 						//new array to store all the tokens from input
		int i = 0; 									//counter to use in conditional loops and to get elements of tokenArray
		
		//while loop to print the tokens, stops when token = NULL
		while (token != NULL) {
			tokenArray[i] = token; 				//add token to tokenArray
			i++; 								//increment i
			token = strtok(NULL, delim); 		//moves onto next token
		}

		//if the entered command == one of our delimiters reset to start of loop and doesn't execute anything
		if (tokenArray[0] == NULL) {
			printf(";, &, >, <, | :: these are all delimiters and cannot be whole commands");
			continue;
		}

		//runs internal commands; checks if internal commands returns -1 (error with command), 0 (expect output) or 1 (command not found), if command is found and executed continue to begining original while loop
		if (internalCommands(tokenArray, i, history, historyCount, aliasList, &aliases) < 1) {
			continue;
		}
		
		//runs external commands
		externalCommands(tokenArray, i);
		
	}
	
	//used when command+D is used to exit loop
	if (check_input == NULL) {
		printf("\n"); //prints new line to keep display consistent with using "exit" to terminate loop
	}
	
	restorePath(path); //restore path to original user path

	saveHistory(history, historyCount); //saves history to .hist_list

	//frees remianing history array from memory
	for (int i = 0; i < historyCount; i++) {
		free(history[i]);
	}
	
	saveAlias(aliasList, aliases); //saves aliases to .aliases
	
	//frees remianing aliases and commands from memory
	for (int o = 0; o < aliases; o++) {
        free(aliasList[o].alias);
        free(aliasList[o].command);
    }
	
	printf("Leaving shell...\n");

	return (0); //stops shell running
	
}
