#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
	
	//initialise variables
	char *prompt = "shell $ "; 	//prompt preceding user input
	char input[512]; 		//input from user
	char *check_input = ""; 	//used in comparsions to check user input
	char *path = getenv("HOME");
	
	chdir(getenv("HOME"));

	//infinte loop, terminates when conditional is tripped
	while (1) {
		
		printf("%s", prompt); //prints prompt
		check_input = fgets(input, sizeof(input), stdin); //(fgets) gets input from user, (check_input) used to check if the input is NULL i.e. command+D
		
		//conditional checking if command+D or "exit" has been inputted
		if (check_input == NULL || strcmp(check_input, "exit\n" ) == 0) {
			break; //terminates loop
		}
		
		check_input[strlen(check_input)-1] = '\0'; 	//takes out the \n (newline) char from end of string
		
		char* delim = " \t\n;&><|";			//new variable to be used as a delimiter
		char* token = strtok(check_input, delim);	//new variable to store the first token
		char* tokenArray[256]; 				//new array to store all the tokens from input
		int i = 0; 					//counter to use in conditional loops and to get elements of tokenArray
		
		//while loop to print the tokens, stops when token = NULL
		while (token != NULL) {
			tokenArray[i] = token; 			//add token to tokenArray
			printf("{%s}\n", tokenArray[i]); 	//print current element of token array
			i++; 					//increment i
			token = strtok(NULL, delim); 		//moves onto next token
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
	
	return (0);
	
}
