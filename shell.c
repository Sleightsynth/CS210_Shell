#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <libgen.h> // for cd Stage 4

void getPath();
void setPath(char* new_path);
static void shell_cd(const char *path);

int main(void) {
	
	//initialise variables
	char *prompt = "shell $ "; 	//prompt preceding user input
	char input[512]; 		//input from user
	char *check_input = ""; 	//used in comparsions to check user input
	char *path = getenv("PATH");	//saves original path from user 
	
	chdir(getenv("HOME")); 		//sets directory to users home directory
	
	//infinte loop, terminates when conditional is tripped
	while (1) {
		
		printf("%s", prompt); //prints prompt
		check_input = fgets(input, sizeof(input), stdin); //(fgets) gets input from user, (check_input) used to check if the input is NULL i.e. command+D
		
		//conditional checking if command+D or "exit" has been inputted
		if (check_input == NULL || strcmp(check_input, "exit\n") == 0) {
			break; //terminates loop
		} else if (strcmp(check_input, "\n") == 0) {
			continue;
		}
		
		check_input[strlen(check_input)-1] = '\0'; 	//takes out the \n (newline) char from end of string 
		
		char* delim = " \t\n;&><|";			//new variable to be used as a delimiter
		char* token = strtok(check_input, delim);	//new variable to store the first token
		char* tokenArray[256]; 				//new array to store all the tokens from input
		int i = 0; 					//counter to use in conditional loops and to get elements of tokenArray
		
		if (strncmp(token, "cd", 2) == 0 && token[2] == '\0') {
            		path = &token[3]; // uses character after CD function for address and sets to path.
            		shell_cd(path); // uses copied address and shell_cd function to change path.
            		continue;
        	}

		
		//while loop to print the tokens, stops when token = NULL
		while (token != NULL) {
			tokenArray[i] = token; 			//add token to tokenArray
			printf("{%s}\n", tokenArray[i]); 	//print current element of token array
			i++; 					//increment i
			token = strtok(NULL, delim); 		//moves onto next token
		}
		
		
		if (strcmp(tokenArray[0], "getpath") == 0) {
			if (i == 1) {
				getPath();
				continue;				
			}
			printf("Failed to get path: too many arguements\n");
			continue;
		} else if (strcmp(tokenArray[0], "setpath") == 0) {
			if (i == 1) {
				printf("Failed to set path: no path provided\n");
				continue;
			} else if (i > 2) {
				printf("Failed to set path: too many arguements\n");
				continue;
			}
			setPath(tokenArray[1]);
			//printf("%s\n", getenv("PATH"));
			continue;
		}
		
		tokenArray[i] = NULL; 	//Null terminator at the end of the loop, allowing our token array to be used in execvp
		pid_t pid = fork(); 	// creating the child process
		
		//conditional to excute child or parent process
		if (pid < 0) { //neither parent or child - error
			printf("error\n");
			return(-1);
		} else if (pid == 0) { //Child process
			execvp(tokenArray[0], tokenArray); 	//exec to make child process exceute a different process than parent
			perror(tokenArray[0]); 			//returns the exact error from exec, if one has occured
			exit(1);
		} else { //parent process, shell
			wait(NULL); 				//makes parent process wait for child process to finish it's execution
		}
		
	}
	
	//used when command+D is used to exit loop
	if (check_input == NULL) {
		printf("\n"); //prints new line to keep display consistent with using "exit" to terminate loop
	}
	
	printf("Leaving shell...\n");
	
	setenv("PATH", path, 1);
	printf("%s\n", getenv("PATH"));
	
	return (0);
	
}

void getPath() {
	
	printf("%s\n", getenv("PATH"));
	return;	
	
}

void setPath(char* new_path) {

	setenv("PATH", new_path, 1);
	perror("setenv");
	return;
	
}

static void shell_cd(const char *path) {

    int result = 0; //initialise result to 0.

    // handles case if path is NULL or ".".
    if (path == NULL || strcmp(path, ".") == 0) {
        char* buf = getcwd(NULL,0); //retrieves current working directory.
        if (buf == NULL) {
            perror("Failed to retrieve current working directory."); //prints error message if path is null.
            return;
        }
        printf("%s\n", buf); //prints current working directory.
        free(buf); // frees memory for buf after use.
    // handles case if path is "..".
    } else if ((strcmp(path, "..") == 0)) {
        //Move to parent directory.
        char *buf = getcwd(NULL, 0); //retrieves current working directory.
        if (buf == NULL) {
            perror("Failed to retrieve current working directory."); //prints error message if unable to retrieve current working directory.
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
        result = chdir(path);
    }

    // confirms if directory was changed successfully.
    if (result != 0) {
        perror("Failed to change directory"); // prints an error message if directory was not changed.
    }
}
