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
		} else {
			perror("Failed to retrieve current working directory."); //prints error message if path is null.
			return(-1);
		}
	} else if (strcmp(tokenArray[0], "history") == 0) {
			showHistory(history, historyCount);
			return(0);
	}

    else if (strcmp(tokenArray[0], "alias") == 0) {
        if (i == 1) {
            showAliases(aliasList, *aliases);
			printf("aliases = %d\n", *aliases);
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
			printf("command = %s\n", command);
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
	printf("%s\n", getenv("PATH"));
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
        perror("Failed to change directory"); // prints an error message if directory was not changed.
    }
}

//function to add commands from user in terminal to history 
//--calls invokeHistory
char* addToHistory(char *command, char *history[], int *count) { 

	if (command[0] == '!') {
		return invokeHistory(history, command, *count);
		//strcpy(command, (invokeHistory(history, command, *count)));
		if (strcmp(command, "") == 0) {
			return command;
		}
	}

	if (*count < HISTORY_SIZE) {
		history[*count] = strdup(command);
		(*count)++;
		return history[(*count)-1];
	} else {
		for (int i = 0; i < HISTORY_SIZE - 1; i++) {
			free(history[i]);
			history[i] = strdup(history[i + 1]);
		}
		
		history[HISTORY_SIZE - 1] = strdup(command);
		
	} 
	return history[*count-1];
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
	
	if (command[0] == '!' && command [1] == '!' && command[2] == '\0') { 
		if (count == 0) {
			printf("%s: no previos event found\n", command);
			return "";
		}
		return history[count-1];
		
		/*char* new_command = malloc(strlen(history[count-1]) + 1); 
		strcpy(new_command, history[count-1]);
		return strcat(new_command, (command+2));*/

	} else if (command[0] == '!' && command [1] == '-') {

		for (int o = 2; o < strlen(command); o++) {
			if (isalpha(command[o])) {
				printf("%s: event not found\n", command);
				return "";
			}
		}

		int sub_command = atoi(command+2);
		if (sub_command <= 0) {
			printf("%s: event not found\n", command);
			return "";
		}
		if (count - sub_command <= 0) {
			printf("%s: event not found\n", command);
			return "";
		} else return history[(count-1)-sub_command];

	} else {

		for (int o = 1; o < strlen(command); o++) {
			if (isalpha(command[o])) {
				printf("%s: event not found\n", command);
				return "";
			}
		}

		int sub_command = atoi(command+1);
		
		if (count == 0) {
			printf("%s: no previos event found\n", command);
			return "";
		} else if (sub_command <= 0 || sub_command > count) {
			printf("%s: event not found\n", command);
			return "";
		} else { return history[sub_command-1];

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
	file = fopen(".shell_history.hist_list", "w"); //opens history file in write mode

	//loop to write all contents of history to file, stops when all elements of history have been traversed
	for (int o = 0; o < count; o++) {
		fprintf(file, "%s\n", history[o]); //writes element of history to file
		//printf("%s -> added to file\n", history[o]); //temporary print just to see elements that are saved
	}

	fclose(file); //closes file

}

void loadHistory(char* history[], int* count) {

	char* file_name = ".shell_history.hist_list";

	FILE *file;
	file = fopen(file_name, "r");

	if (file == NULL) {
		return;
	}

	char read_item[512];

	while (!feof(file) && !ferror(file)) {
		if (fgets(read_item, 512, file) != NULL) {
			
			int size = strlen(read_item); //Total size of string
			char* str = malloc(sizeof(char)*size);
			strcpy(str, read_item);
			
			str[size-1] = '\0';
			history[*count] = strdup(str);
			(*count)++;
		}
	}
	
	fclose(file);
	
}

// Aliases Stage 7 and 8
int updateAlias (char *command, aliasEntry aliasList[], int count) {
    for (int i = 0; i < count; i++ ) {
        if (strcmp(aliasList[i].alias, command) == 0) {
            char *temp = malloc(strlen(aliasList[i].command) + 1);
            strcpy(temp, aliasList[i].command);
            strcpy(command, temp);
            free(temp);
            return 1;
        }
    }
    return 0;
}

int isAlias(char *string, aliasEntry aliasList[], int value, int count) {
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (value == 1) {
            if (strcmp(aliasList[i].alias, string) == 0) {
                found = 1;
                return found;
            }
        }
        else if (value == 2) {
            if (strcmp(aliasList[i].command, string) == 0) {
                found = 1;
                return found;
            }
        }
    }
    return found;
}

void showAliases(aliasEntry aliasList[], int count) {
    for (int i = 0; i < count; i++) {
        printf("%s     %s\n", aliasList[i].alias, aliasList[i].command);
    }
}

int addAlias(char *newAlias, char *command, aliasEntry aliasList[], int* count){
    if(isAlias(newAlias, aliasList, 1, *count)) {
        for(int i = 0; i < *count; i++) {
            if (strcmp(aliasList[i].alias, newAlias) == 0) {
                free(aliasList[i].command);
                aliasList[i].command = malloc(strlen(command) + 1);
                strcpy(aliasList[i].command, command);
                return *count;
            }
        }
        return *count;
    }
    else {
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

/*
int mainAlias(aliasEntry aliasList[], int argc, char **args, int count){
    int newCount = count;
    if (argc == 1) {
        showAliases(aliasList, count);
        return count;
    }
    else if (argc == 3) {
        newCount = addAlias(args[1],args[2], aliasList, count);
        return newCount;
    }
    else {
        return count;
    }
}
 */

int removeAlias (char *aliasToRemove, aliasEntry aliasList[], int *count) {
    for (int i = 0; i < *count; i++) {
        if (strcmp(aliasList[i].alias, aliasToRemove) == 0) {
            for (int j = i; j < *count - 1; j++) {
                strcpy(aliasList[j].alias, aliasList[j+1].alias);
                strcpy(aliasList[j].command, aliasList[j+1].command);
            }
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

	char* file_name = ".aliases";

	FILE *file;
	file = fopen(file_name, "r");

	if (file == NULL) {
		return;
	}

	char read_item[512];

	while (!feof(file) && !ferror(file)) {
		if (fgets(read_item, 512, file) != NULL) {
			
			char *token = strcpy(read_item, "=");
			if (token != NULL) {
				*token = '\0';
				*aliases = addAlias(read_item, (token+1), aliasList, aliases);
			}
		}
	}
	
	fclose(file);
	
}
