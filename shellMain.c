#include "shellHeader.h"

int main(void) {
	
	char input[512]; 		//input from user
	char *check_input = ""; 	//used in comparsions to check user input
	char *path = getUserPath();	//saves original path from user
	char *history[HISTORY_SIZE];
	int historyCount = 0;

	aliasEntry aliasList[10];
	int aliases = 0;
	
	setHomeDirectory(); //sets directory to users home directory
	loadHistory(history, &historyCount);
	loadAlias(aliasList, &aliases);

	//infinte loop, terminates when conditional is tripped
	while (1) {
		
		printPrompt(); //prints prompt
		check_input = fgets(input, sizeof(input), stdin); //(fgets) gets input from user, (check_input) used to check if the input is NULL i.e. command+D
		
		//conditional checking if command+D or "exit" has been inputted
		if (check_input == NULL || strcmp(check_input, "exit\n" ) == 0) {
			break; //terminates loop
		} else if (strcmp(check_input, "\n") == 0) { //condtional to return to start of loop if only enter is inputted
			continue; //returns to begining of loop
		}

		check_input[strlen(check_input)-1] = '\0'; 	//takes out the \n (newline) char from end of string
		//if (strcmp(addToHistory(check_input, history, &historyCount), "") == 0) {
		//	continue;
		//}
		strcpy(check_input, addToHistory(check_input, history, &historyCount));
		if (strcmp(check_input, "") == 0) {
			continue;
		}
		
		if (isAlias(check_input, aliasList, 1, aliases)) {
            if (updateAlias(check_input,aliasList,aliases) == 0) {
                perror("Alias Error.");
            }
        }
		
		char* delim = " \t\n;&><|";			//new variable to be used as a delimiter
		char* token = strtok(check_input, delim);	//new variable to store the first token
		char* tokenArray[256]; 				//new array to store all the tokens from input
		int i = 0; 					//counter to use in conditional loops and to get elements of tokenArray

		//free(check_input);
		
		//while loop to print the tokens, stops when token = NULL
		while (token != NULL) {
			tokenArray[i] = token; 			//add token to tokenArray
			//printf("{%s}\n", tokenArray[i]); 	//print current element of token array
			i++; 					//increment i
			token = strtok(NULL, delim); 		//moves onto next token
		}
		
		if (internalCommands(tokenArray, i, history, historyCount, aliasList, &aliases) < 1) {
			continue;
		}
		
		externalCommands(tokenArray, i);
		
	}
	
	//used when command+D is used to exit loop
	if (check_input == NULL) {
		printf("\n"); //prints new line to keep display consistent with using "exit" to terminate loop
	}
	
	printf("Leaving shell...\n");
	
	setenv("PATH", path, 1);
	//getPath();

	saveHistory(history, historyCount);

	for (int i = 0; i < historyCount; i++) {
		free(history[i]);
	}
	
	saveAlias(aliasList, aliases);
	
	for (int o = 0; o < aliases; o++) {
        printf("Alias %d: %s=%s\n", (o+1), aliasList[o].alias, aliasList[o].command);
        free(aliasList[o].alias);
        free(aliasList[o].command);
    }
	
	return (0);
	
}
