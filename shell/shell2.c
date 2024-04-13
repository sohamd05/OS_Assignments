#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int main() {
    int ret;
    char input[128];
    char path[128] = "/usr/bin:/bin:/sbin";

    while (1) {
        updatePrompt();  // Update prompt before printing
        printf("%s", prompt);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  // Add a newline for better formatting
            break;         // Exit the loop on Ctrl+D
        }

        input[strcspn(input, "\n")] = '\0';  // Remove newline character

        if (strcmp(input, "exit") == 0)
            return 0;

        if (strncmp(input, "PS1=", 4) == 0) {
            changePrompt(input + 4); // Start from the 5th character after 'PS1='
            continue;
        }

        if (strcmp(input, "PATH") == 0) {
            fgets(path, sizeof(path), stdin);
            path[strcspn(path, "\n")] = '\0';  // Remove newline character
            continue;
        }

        if (strncmp(input, "cd ", 3) == 0) {
            char dir[128];
            strcpy(dir, input + 3);
            if (chdir(dir) != 0) {
                printf("cd: %s: No such file or directory\n", dir);
            }
            continue;
        }

        int pid = fork();

        if (pid == 0) {
            char* token;
            char* args[128];
            int i = 0;

            token = strtok(input, " ");
            while (token != NULL) {
                args[i++] = token;
                token = strtok(NULL, " ");
            }
            args[i] = NULL;

            char execPath[128];
            token = strtok(path, ":");
            while (token != NULL) {
                strcpy(execPath, token);
                strcat(execPath, "/");
                strcat(execPath, args[0]);
                if (access(execPath, X_OK) == 0) {
                    ret = execv(execPath, args);
                    if (ret == -1) {
                        printf("execv error\n");
                        exit(1);
                    }
                    break;
                }
                token = strtok(NULL, ":");
            }

            if (token == NULL) {
                printf("%s: command not found\n", args[0]);
                exit(1);
            }
        } else {
            wait(0);
        }
    }
    return 0;
}
