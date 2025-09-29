#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include <lab1/messages.h>

#define MAX_BUFFER_SIZE 4096
#define PROBABILITY 80

ssize_t ReadFilename(char *buffer) {
    ssize_t size = read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE);

    if (size == -1) {
        char message[] = "[ERROR] Can`t read filename for process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return size;
    }

    buffer[size - 1] = '\0';
    --size;

    return size;
}

ssize_t WriteLine(int pipe1,
                  int pipe2,
                  char *line,
                  uint64_t *line_size) {
    line[*line_size] = '\n';
    ++*line_size;

    int pipe_to_send;

    if (rand() % 100 < PROBABILITY) {
        pipe_to_send = pipe1;
    } else {
        pipe_to_send = pipe2;
    }

    ssize_t written = write(pipe_to_send, line, *line_size);

    *line_size = 0;

    return written;
}

int32_t ProcessInput(int child1_write_pipe,
                     int child2_write_pipe) {
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
                WriteLine(child1_write_pipe, child2_write_pipe, line, &line_size);

                continue;
            }

            line[line_size] = buffer[i];
            ++line_size;
        }
    }

    if (line_size > 0) {
        WriteLine(child1_write_pipe, child2_write_pipe, line, &line_size);
    }

    return 0;
}

int main(void) {
    char filename1[MAX_BUFFER_SIZE], filename2[MAX_BUFFER_SIZE];

    if (ReadFilename(filename1) == -1) {
        return 1;
    }

    if (ReadFilename(filename2) == -1) {
        return 1;
    }

    int child1_pipes[2], child2_pipes[2];

    if (pipe(child1_pipes) == -1) {
        char message[] = "[ERROR] Can`t create pipes for first child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    if (pipe(child2_pipes) == -1) {
        close(child1_pipes[0]);
        close(child1_pipes[1]);

        char message[] = "[ERROR] Can`t create pipes for second child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == -1) {
        close(child1_pipes[0]);
        close(child1_pipes[1]);
        close(child2_pipes[0]);
        close(child2_pipes[1]);

        char message[] = "[ERROR] Can`t create first child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    // child1
    if (pid1 == 0) {
        dup2(child1_pipes[0], STDIN_FILENO);
        execl("./child", filename1, NULL);

        return 0;
    }

    pid2 = fork();

    if (pid2 == -1) {
        write(child1_pipes[1], EXIT_MESSAGE, sizeof(EXIT_MESSAGE));

        waitpid(pid1, NULL, 0);

        close(child1_pipes[0]);
        close(child1_pipes[1]);
        close(child2_pipes[0]);
        close(child2_pipes[1]);

        char message[] = "[ERROR] Can`t create second child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    // child2
    if (pid2 == 0) {
        dup2(child2_pipes[0], STDIN_FILENO);
        execl("./child", filename2, NULL);

        return 0;
    }

    // parent
    int result = ProcessInput(child1_pipes[1],
                              child2_pipes[1]);

    write(child1_pipes[1], EXIT_MESSAGE, sizeof(EXIT_MESSAGE));
    write(child2_pipes[1], EXIT_MESSAGE, sizeof(EXIT_MESSAGE));

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    close(child1_pipes[0]);
    close(child1_pipes[1]);
    close(child2_pipes[0]);
    close(child2_pipes[1]);

    return result;
}
