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

//function for internal commands
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
			setHomeDirectory();
			return(0);
		} else if (i > 2) {
			printf("Failed to retrieve current working directory: too many directories provided\n");
			return(-1);
		}
	} else if (strcmp(tokenArray[0], "history") == 0) {
		if (i == 1) {
			showHistory(history, historyCount);
			return(0);
		} else {
			printf("Could not retrieve history: history does not take any parameters\n");
			return (-1);
		}
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
            printf("Invalid input, Please enter an Alias name and Command.\n");
            return(-1);
        }
    } else if (strcmp(tokenArray[0], "unalias") == 0) {
		
		if (*aliases == 0) {
			printf("Could not remove alias: no current aliases exist\n");
			return (-1);
		} else if (i == 2) {
            removeAlias(tokenArray[1],aliasList, aliases);
            return (0);
        } else if (i == 1) {
            printf("Invalid input, Please enter an Alias to remove.\n");
			return (-1);
        } else {
			printf("Could not remove alias: too many arguements provided -- aliases are only one word\n");
			return (-1);
		}
    }
	return(1);
}

//function for external commands
int externalCommands(char* tokenArray[], int i) {

	tokenArray[i] = NULL; 	//null terminator at the end of the loop, allowing our token array to be used in execvp
	pid_t pid = fork(); 	//creating the child process
		
	//conditional to excute child or parent process
	if (pid < 0) { //neither parent or child - error
		printf("error\n");
		return (-1);
	} else if (pid == 0) { //child process
		execvp(tokenArray[0], tokenArray); 	//exec to make child process exceute a different process than parent
		perror(tokenArray[0]); 			  	//returns the exact error from exec, if one has occured
		exit(1);
	} else { 
		//parent process, shell
		wait(NULL); //makes parent process wait for child process to finish it's execution
	}
	
	return (0);
	
}

//function to restore the path back to original user path, takes a path as it's only parameter
void restorePath(char* path) {

	printf("Current path -> ");
	getPath();
	setenv("PATH", path, 1);
	printf("Restored path -> ");
	getPath();
	return;
	
}

//function to get current user path 
void getPath() {
	
	printf("%s\n", getenv("PATH"));
	return;	
	
}

//function to change the user path to value inputted by user, takes a char pointer new_path from user input as only parameter
void setPath(char* new_path) {

	setenv("PATH", new_path, 1);
	return;
	
}

//function to implement cd functionality, allowing user to change current working directory
void changeDirectory(const char *directory) {

    int result = 0; //initialise result to 0

    //handles case if path is NULL or "."
    if (directory == NULL || strcmp(directory, ".") == 0) {
        char* buf = getcwd(NULL,0); //retrieves current working directory
        if (buf == NULL) {
            //perror("Failed to retrieve current working directory."); //prints error message if path is null
			printf("Failed to retrieve current working directory.");
            return;
        }
        printf("%s\n", buf); //prints current working directory
        free(buf); 	//frees memory for buf after use
    //handles case if path is ".."

    } else if ((strcmp(directory, "..") == 0)) {
        //move to parent directory.
        char *buf = getcwd(NULL, 0); //retrieves current working directory
        if (buf == NULL) {
            //perror("Failed to retrieve current working directory."); //prints error message if unable to retrieve current working directory
			printf("Failed to retrieve current working directory.");
            return;
        }
        //changes to parent directory
        result = chdir(dirname(buf));
        free(buf);
        if (result != 0) {
            perror("Failed to move to parent directory."); //prints error message if unable to move to parent directory
            return;
        }
    } else {
        //changes to specified directory
        result = chdir(directory);
    }

    //confirms if directory was changed successfully.
    if (result != 0) {
		fprintf(stderr, "Failed to change directory: %s: ", directory);
		perror(0);
        //printf("Failed to change directory: %s", directory); // prints an error message if directory was not changed
    }
}
