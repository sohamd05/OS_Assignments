#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_HISTORY_SIZE 20
#define MAX_BACKGROUND_JOBS 10

char cwd[128];
char history[MAX_HISTORY_SIZE][MAX_INPUT_SIZE];
int history_count = 0;

pid_t background_jobs[MAX_BACKGROUND_JOBS];
int background_job_count = 0;

void updateCWD() {
    getcwd(cwd, sizeof(cwd));
}

void handle_sigint(int sig) {
    printf("\n");
    updateCWD();
    printf("%s", cwd);
    fflush(stdout);
}

void handle_sigstop(int sig) {
    printf("\n");
    updateCWD();
    printf("%s", cwd);
    fflush(stdout);
}

void handle_sigtstp(int sig) {
    // Print message
    printf("\nCtrl+Z pressed. Suspending foreground process...\n");
    // Suspend the foreground process
    kill(0, SIGTSTP);
    // Print the prompt
    updateCWD();
    printf("%s", cwd);
    fflush(stdout);
}

void fg_command() {
    if (background_job_count > 0) {
        pid_t pid = background_jobs[background_job_count - 1];
        background_job_count--;

        // Bring the background process to the foreground
        kill(pid, SIGCONT);
        
        // Wait for the foreground job to finish executing
        int status;
        waitpid(pid, &status, WUNTRACED);
        
        // Check if the foreground job was stopped by a signal
        if (WIFSTOPPED(status)) {
            printf("Foreground job stopped.\n");
        }
    } else {
        printf("No background jobs\n");
    }
}

void bg_command() {
    if (background_job_count > 0) {
        pid_t pid = background_jobs[background_job_count - 1];

        // Resume the background process
        kill(pid, SIGCONT);
        
        printf("Resumed background job with PID: %d\n", pid);
    } else {
        printf("No background jobs\n");
    }
}

int main() {
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigstop);
    signal(SIGTSTP, handle_sigtstp);  // Register the handler for SIGTSTP

    int ret;
    char input[MAX_INPUT_SIZE];
    char path[256]; // Increased buffer size for multiple paths
    strcpy(path, "/usr/bin:/bin:/sbin"); // Default paths

    while (1) {
        updateCWD();  // Update cwd before printing
        printf("%s $>> ", cwd);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");  // Add a newline for better formatting
            break;         // Exit the loop on Ctrl+D
        }

        input[strcspn(input, "\n")] = '\0';  // Remove newline character

        if (strcmp(input, "exit") == 0)
            return 0;

        if (strcmp(input, "clear") == 0) {
            system("clear");
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

        if (strcmp(input, "history") == 0) {
            for (int i = 0; i < history_count; ++i) {
                printf("%d: %s\n", i + 1, history[i]);
            }
            continue;
        }

        if (strcmp(input, "fg") == 0) {
            fg_command();
            continue;
        }

        if (strcmp(input, "bg") == 0) {
            bg_command();
            continue;
        }

        if (history_count < MAX_HISTORY_SIZE) {
            strcpy(history[history_count], input);
            history_count++;
        } else {
            for (int i = 0; i < MAX_HISTORY_SIZE - 1; ++i) {
                strcpy(history[i], history[i + 1]);
            }
            strcpy(history[MAX_HISTORY_SIZE - 1], input);
        }

        char *command = strtok(input, "|");
        int num_pipes = 0;
        char *commands[10]; // Array to store individual commands after splitting by pipe '|'
        while (command != NULL) {
            commands[num_pipes++] = command;
            command = strtok(NULL, "|");
        }
        commands[num_pipes] = NULL;

        // Forking child processes for each command
        int pipes[num_pipes - 1][2];
        for (int i = 0; i < num_pipes - 1; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("pipe error");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < num_pipes; i++) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork error");
                exit(EXIT_FAILURE);
            } else if (pid == 0) { // Child process
                // Redirecting input for all commands except the first one
                if (i != 0) {
                    if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }
                // Redirecting output for all commands except the last one
                if (i != num_pipes - 1) {
                    if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }

                // Closing all pipe ends
                for (int j = 0; j < num_pipes - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                char *token;
                char *args[128];
                int arg_count = 0;

                token = strtok(commands[i], " ");
                while (token != NULL) {
                    args[arg_count++] = token;
                    token = strtok(NULL, " ");
                }
                args[arg_count] = NULL;

                char execPath[128];
                char *path_copy = strdup(path);
                token = strtok(path_copy, ":");
                while (token != NULL) {
                    strcpy(execPath, token);
                    strcat(execPath, "/");
                    strcat(execPath, args[0]);
                    if (access(execPath, X_OK) == 0) {
                        ret = execv(execPath, args);
                        if (ret == -1) {
                            printf("execv error\n");
                            exit(EXIT_FAILURE);
                        }
                        break;
                    }
                    token = strtok(NULL, ":");
                }

                if (token == NULL) {
                    printf("%s: command not found\n", args[0]);
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Closing all pipe ends in the parent process
        for (int i = 0; i < num_pipes - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        // Waiting for all child processes to finish
        for (int i = 0; i < num_pipes; i++) {
            wait(NULL);
        }
    }
    return 0;
}
