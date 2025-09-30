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

int32_t ProcessInput(int child1_write_pipe,
                     int child2_write_pipe) {
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read = 0;

    while ((bytes_read = read(STDIN_FILENO, buffer, MAX_BUFFER_SIZE)) > 0) {
        if (strncmp(buffer, EXIT_MESSAGE, sizeof(EXIT_MESSAGE) - 1) == 0) {
            break;
        }

        int pipe_to_send;

        if (rand() % 100 < PROBABILITY) {
            pipe_to_send = child1_write_pipe;
        } else {
            pipe_to_send = child2_write_pipe;
        }

        ssize_t written = write(pipe_to_send, buffer, bytes_read);

        if (written != bytes_read) {
            char message[] = "[ERROR] Can`t send all text from parent to child process!\n";
            write(STDOUT_FILENO, message, sizeof(message));

            break;
        }
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
    int32_t result = ProcessInput(child1_pipes[1],
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
