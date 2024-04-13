#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGUMENTS 64

// Global variables
char prompt[MAX_COMMAND_LENGTH]; // Stores the prompt string
char* path; // Stores the PATH variable

// Function to parse the command line into arguments
void parseCommand(char* command, char** arguments) {
    char* token = strtok(command, " ");
    int i = 0;
    while (token != NULL) {
        arguments[i++] = token;
        token = strtok(NULL, " ");
    }
    arguments[i] = NULL;
}

// Function to execute the command
void executeCommand(char** arguments) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execv(arguments[0], arguments);
        perror("execv"); // Print error message if execv fails
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        wait(NULL); // Wait for the child process to finish
    } else {
        // Fork failed
        perror("fork"); // Print error message if fork fails
        exit(EXIT_FAILURE);
    }
}

// Function to handle built-in commands
int handleBuiltInCommands(char** arguments) {
    if (strcmp(arguments[0], "exit") == 0) {
        // Exit the shell
        return 1;
    } else if (strcmp(arguments[0], "cd") == 0) {
        // Change directory
        if (arguments[1] == NULL) {
            // No directory specified, go to home directory
            chdir(getenv("HOME"));
        } else {
            if (chdir(arguments[1]) != 0) {
                perror("chdir"); // Print error message if chdir fails
            }
        }
    } else if (strcmp(arguments[0], "PATH") == 0) {
        // Set or print the PATH variable
        if (arguments[1] != NULL) {
            // Set the PATH variable
            path = arguments[1];
        } else {
            // Print the current value of PATH variable
            printf("PATH=%s\n", path);
        }
    } else if (strcmp(arguments[0], "PS1") == 0) {
        // Set or print the prompt
        if (arguments[1] != NULL) {
            // Set the prompt
            strcpy(prompt, arguments[1]);
        } else {
            // Print the current prompt
            printf("Prompt: %s\n", prompt);
        }
    }
    return 0;
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char* arguments[MAX_ARGUMENTS];

    // Initialize variables
    strcpy(prompt, "\\w$"); // Set default prompt
    path = getenv("PATH"); // Get the initial value of PATH variable

    while (1) {
        // Print the prompt with current working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s%s ", prompt, cwd);
        } else {
            perror("getcwd"); // Print error message if getcwd fails
            exit(EXIT_FAILURE);
        }

        // Read the command from user
        if (fgets(command, sizeof(command), stdin) == NULL) {
            // Handle Ctrl-D or EOF
            printf("\n");
            break;
        }

        // Remove trailing newline character
        command[strcspn(command, "\n")] = '\0';

        // Parse the command into arguments
        parseCommand(command, arguments);

        // Handle built-in commands
        if (handleBuiltInCommands(arguments)) {
            // Exit the shell
            break;
        }

        // Execute the command
        executeCommand(arguments);
    }

    return 0;
}
