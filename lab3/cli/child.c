#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include <lab1/constants.h>


void InvertString(char *buffer,
                  uint64_t size) {
    for (uint64_t i = 0; i < size / 2; ++i) {
        char tmp = buffer[size - i - 1];

        buffer[size - i - 1] = buffer[i];
        buffer[i] = tmp;
    }
}

void CloseDescriptor(int descriptor) {
    if (close(descriptor) != 0) {
        char message[] = "[ERROR] Can`t close descriptor!\n";
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
    if (sem_close(semaphore) != 0) {
        char message[] = "[ERROR] Can`t close semaphore!\n";
        write(STDOUT_FILENO, message, sizeof(message));
    }
}

int main(int argc,
         char **argv) {
    if (argc != 2) {
        char message[] = "[ERROR] Child process must start with process id and output filename arguments!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    char *id = argv[0];
    int file = open(argv[1],
                    O_RDWR | O_CREAT,
                    S_IRWXU);

    if (file == -1) {
        char message[] = "[ERROR] Can`t create or open file in child process!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    int shared_memory = shm_open(SHARED_MEMORY_NAME,
                                 O_RDWR,
                                 0);

    if (shared_memory == -1) {
        CloseDescriptor(file);

        char message[] = "[ERROR] Can`t create shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    char *buffer = mmap(NULL,
                        MAX_BUFFER_SIZE,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        shared_memory,
                        0);

    if (buffer == MAP_FAILED) {
        CloseDescriptor(shared_memory);
        CloseDescriptor(file);

        char message[] = "[ERROR] Can`t map shared memory!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    sem_t *semaphore = sem_open(SEMAPHORE_NAME,
                                O_RDWR);

    if (semaphore == SEM_FAILED) {
        UnmapMemory(buffer, MAX_BUFFER_SIZE);
        CloseDescriptor(shared_memory);
        CloseDescriptor(file);

        char message[] = "[ERROR] Can`t create semaphore!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    char text[MAX_MESSAGE_SIZE + 1] = {0};

    bool running = true;
    while (running) {
        sem_wait(semaphore);

        if (strncmp(buffer, EXIT_MESSAGE, sizeof(EXIT_MESSAGE) - 1) == 0) {
            sem_post(semaphore);

            running = false;

            continue;
        }

        if (buffer[0] != id[0]) {
            sem_post(semaphore);

            usleep(100000);

            continue;
        }

        memcpy(text, buffer + 1, MAX_MESSAGE_SIZE);
        memset(buffer, 0, MAX_BUFFER_SIZE);

        sem_post(semaphore);

        size_t length = strlen(text);

        if (text[length - 1] == '\n') {
            InvertString(text, length - 1);
        } else {
            InvertString(text, length);
        }

        ssize_t written = write(file, text, length);

        if (written != (ssize_t) length) {
            char message[] = "[ERROR] Can`t write all text to file!\n";
            write(STDOUT_FILENO, message, sizeof(message));

            running = false;

            continue;
        }
    }

    CloseSemaphore(semaphore);
    UnmapMemory(buffer, MAX_BUFFER_SIZE);
    CloseDescriptor(shared_memory);
    CloseDescriptor(file);

    return 0;
}
