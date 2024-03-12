#include "shellHeader.h"

//function to set the users directory to their home directory
void setHomeDirectory() {

	chdir(getenv("HOME"));
	return;
	
} 

//function to get the users current path
char* getUserPath() {

	char *path = getenv("PATH");
	return path;
	
}

//function to print the shell's prompt
void printPrompt() {

	char *prompt = "shell $ ";
	printf("%s", prompt);
	return;
	
}

//
int internalCommands(char* tokenArray[], int i, char* history[], int historyCount, aliasEntry aliasList[], int* aliases) {

	if (strcmp(tokenArray[0], "getpath") == 0) {
		if (i == 1) {
			getPath();
			return(0);				
		}
		printf("Failed to get path: too many arguements\n");
		return(-1);
	} else if (strcmp(tokenArray[0], "setpath") == 0) {
		if (i == 1) {
			printf("Failed to set path: no path provided\n");
			return(-1);
		} else if (i > 2) {
			printf("Failed to set path: too many arguements\n");
			return(-1);
		} else { 
			setPath(tokenArray[1]);
			return(0);
		}
		
	} else if (strcmp(tokenArray[0], "cd") == 0) {
		if (i == 2) {
			char* directory = tokenArray[1];
			changeDirectory(directory);
			return(0);
		} else if (i == 1) {
			//perror("Failed to retrieve current working directory."); //prints error message if path is null.
			printf("Failed to retrieve current working directory: no directory provided\n");
			return(-1);
		} else {
			printf("Failed to retrieve current working directory: too many directories provided\n");
			return(-1);
		}
	} else if (strcmp(tokenArray[0], "history") == 0) {
			showHistory(history, historyCount);
			return(0);
	}

    else if (strcmp(tokenArray[0], "alias") == 0) {
        if (i == 1) {
            showAliases(aliasList, *aliases);
            return (0);
        }
        if (i > 2) {
        	char command[512] = "";
			
        	for (int o = 2; o < i; o++) {
        		strcat(command, tokenArray[o]);
        		if (o+1 != i) {
        			strcat(command, " ");
        		}
        	}
            *aliases = addAlias(tokenArray[1], command, aliasList, aliases);
            return (0);
        } else {
            perror("Invalid input, Please enter an Alias name and Command.");
            return(-1);
        }
    } else if (strcmp(tokenArray[0], "unalias") == 0) {
        if (i == 2) {
            removeAlias(tokenArray[1],aliasList, aliases);
            return (0);
        } else {
            perror("Invalid input, Please enter an Alias to remove.");
        }
    }
	return(1);
}

//
int externalCommands(char* tokenArray[], int i) {

	tokenArray[i] = NULL; 	//Null terminator at the end of the loop, allowing our token array to be used in execvp
	pid_t pid = fork(); 	// creating the child process
		
	//conditional to excute child or parent process
	if (pid < 0) { //neither parent or child - error
		printf("error\n");
		return (-1);
	} else if (pid == 0) { //Child process
		execvp(tokenArray[0], tokenArray); 	//exec to make child process exceute a different process than parent
		perror(tokenArray[0]); 			//returns the exact error from exec, if one has occured
		exit(1);
	} else { //parent process, shell
		wait(NULL); 				//makes parent process wait for child process to finish it's execution
	}
	
	return (0);
	
}

//function to restore the path back to original user path
//--takes a path as it's only parameter
void restorePath(char* path) {

	setenv("PATH", path, 1);
	return;
	
}

//function to get current user path 
void getPath() {
	
	printf("%s\n", getenv("PATH"));
	return;	
	
}

//function to change the user path to value inputted by user
//--takes a char pointer new_path from user input as only parameter
void setPath(char* new_path) {

	setenv("PATH", new_path, 1);
	perror("setenv");
	return;
	
}

//function to implement cd functionality, allowing user to change current working directory
void changeDirectory(const char *directory) {

    int result = 0; //initialise result to 0.

    // handles case if path is NULL or ".".
    if (directory == NULL || strcmp(directory, ".") == 0) {
        char* buf = getcwd(NULL,0); //retrieves current working directory.
        if (buf == NULL) {
            //perror("Failed to retrieve current working directory."); //prints error message if path is null.
			printf("Failed to retrieve current working directory.");
            return;
        }
        printf("%s\n", buf); //prints current working directory.
        free(buf); // frees memory for buf after use.
    // handles case if path is "..".
    } else if ((strcmp(directory, "..") == 0)) {
        //Move to parent directory.
        char *buf = getcwd(NULL, 0); //retrieves current working directory.
        if (buf == NULL) {
            //perror("Failed to retrieve current working directory."); //prints error message if unable to retrieve current working directory.
			printf("Failed to retrieve current working directory.");
            return;
        }
        //changes to parent directory.
        result = chdir(dirname(buf));
        free(buf);
        if (result != 0) {
            perror("Failed to move to parent directory."); //prints error message if unable to move to parent directory.
            return;
        }
    } else {
        // changes to specified directory.
        result = chdir(directory);
    }

    // confirms if directory was changed successfully.
    if (result != 0) {
		fprintf(stderr, "Failed to change directory: %s: ", directory);
		perror(0);
        //printf("Failed to change directory: %s", directory); // prints an error message if directory was not changed.
		//perror(0);
    }
}

//function to add commands from user in terminal to history 
//--calls invokeHistory
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

//
char* invokeHistory(char* history[], char* command, int count) {
	
	if (count == 0) {
			printf("%s: event not found: history has no contents\n", command);
			return "";
	}

	if (command[0] == '!' && command [1] == '!') { 
		
		if (command[2] != '\0') {
			printf("%s: event not found: no arguements expected after '!!'\n", command);
			return "";
		} else if (count == 0) {
			printf("%s: event not found: history has no contents\n", command);
			return "";
		}
		return history[count-1];
		
		/*char* new_command = malloc(strlen(history[count-1]) + 1); 
		strcpy(new_command, history[count-1]);
		return strcat(new_command, (command+2));*/

	} else if (command[0] == '!' && command [1] == '-') {

		for (int o = 2; o < strlen(command); o++) {
			if (isalpha(command[o])) {
				printf("%s: event not found: only numbers expected\n", command);
				return "";
			}
		}

		int sub_command = atoi(command+2);
		if (sub_command <= 0) {
			printf("%s: event not found: arguement must be a negative number\n", command);
			return "";
		}
		if (count - sub_command <= 0) {
			printf("%s: event not found: invocation too low -- (invocation: %d; not in history)\n", command, (count - sub_command));
			return "";
		} else return history[(count)-sub_command];

	} else {

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
		} else if (sub_command > count) {
			printf("%s: event not found: invocation outwidth history contents (current size: %d)\n", command, count);
			return "";
		} else { 
			
			return history[sub_command-1];

			/*char* new_command = malloc(strlen(history[count-1]) + 1); 
			strcpy(new_command, history[sub_command-1]);
			return strcat(new_command, (command+1));*/

		}

	}

}

//function to save all elements of history array to file
void saveHistory(char* history[], int count) {

	setHomeDirectory(); //sets directory to users home directory

	FILE *file; //decleration of file
	file = fopen(".hist_list", "w"); //opens history file in write mode

	//loop to write all contents of history to file, stops when all elements of history have been traversed
	for (int o = 0; o < count; o++) {
		fprintf(file, "%s\n", history[o]); //writes element of history to file
		//printf("%s -> added to file\n", history[o]); //temporary print just to see elements that are saved
	}

	fclose(file); //closes file

}

//function to load users history -- (fill history array with contents of .hist_list file)
void loadHistory(char* history[], int* count) {

	char* file_name = ".hist_list"; 

	FILE *file; //decleration of file
	file = fopen(file_name, "r"); //opens history file in read mode

	//if statement to detect if file does not exist -- i.e. fopen returns NULL
	if (file == NULL) {
		return; //does not load anything into history array
	}

	char read_item[512]; //varaiable used to read lines from .hist_list file

	//loop to add all lines from file to history, stops when it reaches end of file or there has been an error in reading
	while (!feof(file) && !ferror(file)) { 
		if (fgets(read_item, 512, file) != NULL) { //
			
			int size = strlen(read_item); //Total size of string
			char* str = malloc(sizeof(char)*size);
			strcpy(str, read_item);
			
			str[size-1] = '\0';
			history[*count] = strdup(str);
			(*count)++;
		}
	}
	
	fclose(file); //closes file
	
}

// Aliases Stage 7 and 8
// Function to change alias if alias already exists.
int updateAlias (char *command, aliasEntry aliasList[], int count) {
    //Loops through aliasList[].
    for (int i = 0; i < count; i++ ) {
        // if alias is found within array. It will create a temporary variable and update the respective alias to reflect the new command.
        if (strcmp(aliasList[i].alias, command) == 0)  {
            char *temp = malloc(strlen(aliasList[i].command) + 1);
            strcpy(temp, aliasList[i].command);
            strcpy(command, temp);
            free(temp);
            return 1;
        }
    }
    return 0;
}
//Helper function for addAlias to confirm if Alias exists in the array.
int isAlias(char *string, aliasEntry aliasList[], int value, int count) {
    int found = 0;
    // Loops through Alias List
    for (int i = 0; i < count; i++) {
        if (value == 1) {
            // If alias is found in aliasList[], return 1.
            if (strcmp(aliasList[i].alias, string) == 0) {
                found = 1;
                return found;
            }
        }
        else if (value == 2) {
            // if command is also found in alias list, return 1.
            if (strcmp(aliasList[i].command, string) == 0) {
                found = 1;
                return found;
            }
        }
    }
    return found;
}
// Function to show all aliases on file.
void showAliases(aliasEntry aliasList[], int count) {
	if (count == 0) {
        // Print error message if aliasList[] is empty.
		printf("Could not retrieve aliases: no current aliases\n");
		return;
	}
    for (int i = 0; i < count; i++) {
        // Print each alias of the list and their respective commands.
        printf("%s     %s\n", aliasList[i].alias, aliasList[i].command);
    }
}

// Function to add alias to array.
int addAlias(char *newAlias, char *command, aliasEntry aliasList[], int* count){

	if (*count == 10) {
        // if aliasList[] has 10 aliases. This error message will print if user tries to add another.
		printf("Could not add alias: number of aliases maxed, try unalias first\n");
		return *count;
	}
        // If Alias exists in aliasList[], if block executes.
    if(isAlias(newAlias, aliasList, 1, *count)) {
        // Loops through aliasList[]
        for(int i = 0; i < *count; i++) {
            // When the alias is found. It is updated with the relevant command.
            if (strcmp(aliasList[i].alias, newAlias) == 0) {
				printf("Updating alias: %s: '%s' -> '%s'\n", aliasList[i].alias, aliasList[i].command, command);
                free(aliasList[i].command);
                aliasList[i].command = malloc(strlen(command) + 1);
                strcpy(aliasList[i].command, command);
                return *count;
            }
        }
        return *count;
    }
    else {
        //Alias is added to aliasList[] and the relevant command.
        aliasEntry *alias = malloc(sizeof(aliasEntry));
        alias->alias = malloc(strlen(newAlias) + 1);
        alias->command = malloc(strlen(command) + 1);

        strcpy(alias->alias, newAlias);
        strcpy(alias->command, command);

        aliasList[*count] = *alias;
        aliasList[*count].ptr = alias;
        (*count)++;
        return *count;
    }
}

// Function to remove aliases from file.
int removeAlias (char *aliasToRemove, aliasEntry aliasList[], int *count) {
    //Loops through aliasList[]
    for (int i = 0; i < *count; i++) {
        //if alias that is to be removed matches one in the aliasList[], Removes the relevant alias and command.
        if (strcmp(aliasList[i].alias, aliasToRemove) == 0) {
            for (int j = i; j < *count - 1; j++) {
                strcpy(aliasList[j].alias, aliasList[j+1].alias);
                strcpy(aliasList[j].command, aliasList[j+1].command);
            }
            // Count decremented to account for removed alias.
            (*count)--;
            return 1;
        }
    }
    return 0;
}

void saveAlias(aliasEntry aliasList[], int aliases) {
	setHomeDirectory(); //sets directory to users home directory

	FILE *file; //decleration of file
	file = fopen(".aliases", "w"); //opens aliases file in write mode

	//loop to write all aliases to file, stops when all aliases have been written to file
	for (int o = 0; o < aliases; o++) {
		fprintf(file, "%s=%s\n", aliasList[o].alias, aliasList[o].command); //writes aliases to file
	
	}

	fclose(file); //closes file
}

void loadAlias(aliasEntry aliasList[], int *aliases) {

	char *file_name = ".aliases";

	FILE *file;
	file = fopen(file_name, "r");

	if (file == NULL) {
		return;
	}

	char read_item[512];
	int o = 0;

	while (o < 512 && fgets(read_item, 512, file) != NULL) {
		
		size_t len = strlen(read_item);
		if (len > 0 && read_item[len-1] == '\n') {
			read_item[len-1] = '\0';
		}
		
		char *token = strchr(read_item, '=');
		if (token != NULL) {
			*token = '\0';
			aliasList[o].alias = strdup(read_item);
			aliasList[o].command = strdup(token+1);
			o++;
		} else {
			aliasList[o].alias = strdup("");
			aliasList[o].command = strdup(read_item);
			o++;
		}
	}
	
	*aliases = o;
	
	fclose(file);
	
}
