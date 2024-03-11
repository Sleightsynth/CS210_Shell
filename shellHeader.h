#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <libgen.h> // for cd Stage 4
#include <ctype.h> //used for stage 5 to check if character is a letter or number
#define HISTORY_SIZE 20

typedef struct aliasEntry {
    char *alias;
    char *command;
    struct aliasEntry *ptr;
} aliasEntry; // Struct for Alias

void setHomeDirectory();
char* getUserPath();
void printPrompt();
int internalCommands(char* tokenArray[], int i, char* history[], int historyCount, aliasEntry aliasList[], int* aliases);
int externalCommands(char* tokenArray[], int i);
void restorePath(char* path);
void getPath();
void setPath(char* new_path);
void changeDirectory(const char *path);
void showHistory(char* history[], int count);
char* addToHistory(char* command, char* history[], int* count);
char* invokeHistory(char* history[], char* command, int count);
void saveHistory(char* history[], int count);
void loadHistory(char* history[], int* count);
int updateAlias (char *command, aliasEntry aliasList[], int count);
int isAlias(char *string, aliasEntry aliasList[], int value, int count);
void showAliases(aliasEntry aliasList[], int count);
int addAlias(char *newAlias, char *command, aliasEntry aliasList[], int* count);
int removeAlias (char *aliasToRemove, aliasEntry aliasList[], int *count);
