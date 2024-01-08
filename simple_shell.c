#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

int main(void) {
    char input[MAX_INPUT_SIZE];

    while (1) {
        // Display prompt
        printf("($) ");

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // Handle end of file (Ctrl+D)
            printf("\n");
            break;
        }

        // Remove newline character from input
        size_t input_length = strlen(input);
        if (input_length > 0 && input[input_length - 1] == '\n') {
            input[input_length - 1] = '\0';
        }

        // Fork a child process
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            // Execute the command using execve
            if (execve(input, NULL, NULL) == -1) {
                perror("execve");
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            // Wait for the child to complete
            wait(NULL);
        }
    }

    return 0;
}
