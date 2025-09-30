#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <lab1/messages.h>

#define MAX_BUFFER_SIZE 4096


void InvertString(char *buffer,
                  uint64_t size) {
    for (uint64_t i = 0; i < size / 2; ++i) {
        char tmp = buffer[size - i - 1];

        buffer[size - i - 1] = buffer[i];
        buffer[i] = tmp;
    }
}

int main(int argc,
         char **argv) {
    if (argc != 1) {
        char message[] = "[ERROR] Child process must start with output filename argument!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    int file = open(argv[0],
                    O_RDWR | O_CREAT,
                    S_IRWXU);

    if (file == -1) {
        char message[] = "[ERROR] Can`t create or open file in child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read = 0;

    while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE)) > 0) {
        if (strncmp(buffer, EXIT_MESSAGE, sizeof(EXIT_MESSAGE) - 1) == 0) {
            break;
        }

        InvertString(buffer, bytes_read - 1);

        ssize_t written = write(file, buffer, bytes_read);

        if (written != bytes_read) {
            char message[] = "[ERROR] Can`t write all text to file!\n";
            write(STDOUT_FILENO, message, sizeof(message));

            break;
        }
    }

    close(file);

    return 0;
}
