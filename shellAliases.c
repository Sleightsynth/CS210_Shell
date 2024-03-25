#include "shellHeader.h"

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

char getAliasCommand(char *string, aliasEntry aliasList[], int count) {
    int found = 0;
    for (int i = 0; i < count; i++) {
            if (strcmp(aliasList[i].alias, string) == 0) {
                found = 1;
                return aliasList[i].command[0];
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

    if (*count == 10 && isAlias(newAlias, aliasList, 1, *count) != 1) {
        // if aliasList[] has 10 aliases. This error message will print if user tries to add another.
        printf("Could not add alias: number of aliases maxed, try unalias first\n");
        return *count;
    }

    char curr_command[512];
    strcpy(curr_command, command);


    while (isAlias(curr_command, aliasList, 1, *count) == 1) {
        if (updateAlias(curr_command,aliasList,*count) == 0) {
            perror("Alias Error.");
        }
        if (strcmp(curr_command, newAlias) == 0) {
            printf("Aliases not added: cycle detected\n");
            return *count;
        }
    }

    printf("command == %s\n", command);

    /*if ((isAlias(command, aliasList, 1, *count) == 1 && isAlias(newAlias, aliasList, 2, *count) == 1) || strcmp(newAlias, command) == 0) {
        printf("Aliases not added: cycle detected\n");
        return *count;
    }*/

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
            printf("Removing alias '%s': command == %s\n", aliasToRemove, aliasList[i].command);
            for (int j = i; j < *count - 1; j++) {
                strcpy(aliasList[j].alias, aliasList[j+1].alias);
                strcpy(aliasList[j].command, aliasList[j+1].command);
            }
            // Count decremented to account for removed alias.
            (*count)--;
            return 1;
        }
    }
    printf("Could not remove alias %s: no matching alias\n", aliasToRemove);
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
            
            if (strcmp(read_item, "") == 0 || strcmp(token+1, "") == 0) {
                printf("Could not read aliases from file: corrupt file\n");
                return;
            }
            
            aliasList[o].alias = strdup(read_item);
            aliasList[o].command = strdup(token+1);
            o++;
        } else {
            return;
            aliasList[o].alias = strdup("");
            aliasList[o].command = strdup(read_item);
            o++;
        }
    }
    
    *aliases = o;
    
    fclose(file);
    
}
