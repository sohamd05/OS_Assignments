#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

char prompt[128] = "prompt>";
char cwd[128];

void updatePrompt() {
    getcwd(cwd, sizeof(cwd));
}

void changePrompt(const char* newPrompt) {
    if (strcmp(newPrompt, "\\u") == 0) {
        char username[128];
        getlogin_r(username, sizeof(username));
        strcpy(prompt, username);
    } else if (strcmp(newPrompt, "\\w$") == 0) {
        updatePrompt();
        strcat(cwd, "$");
        strcpy(prompt, cwd);
    } else {
        strcpy(prompt, newPrompt);
    }
}

void handleCtrlCSignal(int signal) {
    printf("\n");  // Print a newline after Ctrl-C
    fflush(stdout); // Flush stdout
    signal(SIGINT, handleCtrlCSignal);  // Re-enable the handler
}

void executeCommand(char *command) {
    add_history(command);

    int pid = fork();

    if (pid == 0) {
        char *token;
        char *args[128];
        int i = 0;

        token = strtok(command, " ");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }
}

int main() {
    signal(SIGINT, handleCtrlCSignal);  // Handle Ctrl-C

    char *input;

    while ((input = readline(prompt))) {
        if (strcmp(input, "exit") == 0) {
            free(input);
            break;
        }

        if (strncmp(input, "PS1=", 4) == 0) {
            changePrompt(input + 4); // Start from the 5th character after 'PS1='
            free(input);
            continue;
        }

        if (strncmp(input, "cd ", 3) == 0) {
            char dir[128];
            strcpy(dir, input + 3);
            if (chdir(dir) != 0) {
                printf("cd: %s: No such file or directory\n", dir);
            }
            free(input);
            continue;
        }

        executeCommand(input);

        free(input);
    }

    return 0;
}

