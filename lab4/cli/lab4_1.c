#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <lab4/commands.h>

#define MAX_BUFFER_SIZE 1024


int main(void) {
    char command[MAX_BUFFER_SIZE + 1] = {0};
    while (read(STDIN_FILENO, command, MAX_BUFFER_SIZE) > 0) {
        if (strncmp(command, EXIT_COMMAND, strlen(EXIT_COMMAND)) == 0) {
            break;
        }

        char *type = strtok(command, DELIMITERS);

        if (!type) {
            memset(command, 0, MAX_BUFFER_SIZE);

            continue;
        }

        char message[MAX_BUFFER_SIZE + 1] = {0};
        if (strcmp(type, "1") == 0) {
            ProcessCommand1(prime_count, message);
        } else if (strcmp(type, "2") == 0) {
            ProcessCommand2(pi, message);
        } else {
            sprintf(message, "Invalid command!\n");
        }

        write(STDOUT_FILENO, message, strlen(message));

        memset(command, 0, MAX_BUFFER_SIZE);
    }

    return 0;
}
