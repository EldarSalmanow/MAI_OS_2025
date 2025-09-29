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

ssize_t WriteLine(int file,
                  char *line,
                  uint64_t *line_size) {
    InvertString(line, *line_size);

    line[*line_size] = '\n';
    ++*line_size;

    ssize_t written = write(file, line, *line_size);

    *line_size = 0;

    return written;
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
    char line[MAX_BUFFER_SIZE];
    uint64_t line_size = 0;
    ssize_t bytes_read = 0;

    while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';

        if (strcmp(buffer, EXIT_MESSAGE) == 0) {
            break;
        }

        for (ssize_t i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n'
             || line_size + 1 >= MAX_BUFFER_SIZE) {
                WriteLine(file, line, &line_size);

                continue;
            }

            line[line_size] = buffer[i];
            ++line_size;
        }
    }

    if (line_size > 0) {
        WriteLine(file, line, &line_size);
    }

    close(file);

    return 0;
}
