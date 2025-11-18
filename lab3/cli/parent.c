#include <fcntl.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include <lab1/constants.h>

#define PROBABILITY 80


int32_t WriteMessage(char *buffer,
                     const char *text,
                     sem_t *semaphore) {
    sem_wait(semaphore);

    memcpy(buffer, text, strlen(text) + 1);

    sem_post(semaphore);

    return 0;
}

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

int32_t ProcessInput(char *buffer,
                     sem_t *semaphore) {
    char text[MAX_BUFFER_SIZE] = {0};

    while (read(STDIN_FILENO, text + 1, MAX_MESSAGE_SIZE) > 0) {
        if (strncmp(text + 1, EXIT_MESSAGE, sizeof(EXIT_MESSAGE) - 1) == 0) {
            break;
        }

        char process_to_send = rand() % 100 < PROBABILITY ? '1' : '2';

        text[0] = process_to_send;

        WriteMessage(buffer, text, semaphore);

        memset(text, 0, MAX_BUFFER_SIZE);
    }

    return 0;
}

void UnlinkMemory(const char *name) {
    if (shm_unlink(name) != 0) {
        char message[] = "[ERROR] Can`t unlink shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));
    }
}

void UnmapMemory(void *buffer,
                 size_t length) {
    if (munmap(buffer, length) != 0) {
        char message[] = "[ERROR] Can`t unmap shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));
    }
}

void CloseSemaphore(sem_t *semaphore) {
    if (sem_unlink(SEMAPHORE_NAME) != 0) {
        char message[] = "[ERROR] Can`t unlink semaphore!\n";
        write(STDOUT_FILENO, message, sizeof(message));
    }

    if (sem_close(semaphore) != 0) {
        char message[] = "[ERROR] Can`t close semaphore!\n";
        write(STDOUT_FILENO, message, sizeof(message));
    }
}

int main(void) {
    char filename1[MAX_BUFFER_SIZE], filename2[MAX_BUFFER_SIZE];

    if (ReadFilename(filename1) == -1) {
        return 1;
    }

    if (ReadFilename(filename2) == -1) {
        return 1;
    }

    int shared_memory = shm_open(SHARED_MEMORY_NAME,
                                 O_RDWR | O_CREAT | O_TRUNC,
                                 S_IRUSR | S_IWUSR);

    if (shared_memory == -1) {
        char message[] = "[ERROR] Can`t create shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    if (ftruncate(shared_memory, MAX_BUFFER_SIZE) == -1) {
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Can`t truncate shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    char *buffer = mmap(NULL,
                        MAX_BUFFER_SIZE,
                        PROT_WRITE,
                        MAP_SHARED,
                        shared_memory,
                        0);

    if (buffer == MAP_FAILED) {
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Can`t map shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);

    sem_unlink(SEMAPHORE_NAME);

    sem_t *semaphore = sem_open(SEMAPHORE_NAME,
                                O_RDWR | O_CREAT | O_TRUNC,
                                S_IRUSR | S_IWUSR,
                                1);

    if (semaphore == SEM_FAILED) {
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Can`t create semaphore!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == -1) {
        CloseSemaphore(semaphore);
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Can`t create first child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    // child1
    if (pid1 == 0) {
        execl("./child", "1", filename1, NULL);

        CloseSemaphore(semaphore);
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Failed executing of first child!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    pid2 = fork();

    if (pid2 == -1) {
        WriteMessage(buffer, EXIT_MESSAGE, semaphore);

        waitpid(pid1, NULL, 0);

        CloseSemaphore(semaphore);
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Can`t create second child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    // child2
    if (pid2 == 0) {
        execl("./child", "2", filename2, NULL);

        CloseSemaphore(semaphore);
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        UnlinkMemory(SHARED_MEMORY_NAME);

        char message[] = "[ERROR] Failed executing of second child!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    // parent
    int32_t result = ProcessInput(buffer, semaphore);

    WriteMessage(buffer, EXIT_MESSAGE, semaphore);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    CloseSemaphore(semaphore);
    UnmapMemory(buffer, MAX_BUFFER_SIZE);
    UnlinkMemory(SHARED_MEMORY_NAME);

    return result;
}
