#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lab4/commands.h>

#define MAX_BUFFER_SIZE 1024


static PrimeCountFn prime_count_fn;
static PiFn pi_fn;

static const char *IMPL_1_NAME = "./libmathematics_impl1.so";
static const char *IMPL_2_NAME = "./libmathematics_impl2.so";


int set_mathematics_impl(int impl_number, void **library) {
    void *tmp_library = NULL;

    if (impl_number == 1) {
        tmp_library = dlopen(IMPL_1_NAME, RTLD_LOCAL | RTLD_NOW);
    } else if (impl_number == 2) {
        tmp_library = dlopen(IMPL_2_NAME, RTLD_LOCAL | RTLD_NOW);
    } else {
        char message[] = "Invalid impl number!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    if (!tmp_library) {
        char message[] = "Can`t change impl!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    prime_count_fn = (PrimeCountFn) dlsym(tmp_library, "prime_count");

    if (!prime_count_fn) {
        dlclose(tmp_library);

        char message[] = "Can`t find function `prime_count`!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    pi_fn = (PiFn) dlsym(tmp_library, "pi");

    if (!pi_fn) {
        dlclose(tmp_library);

        char message[] = "Can`t find function `pi`!\n";
        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    if (*library) {
        dlclose(*library);
    }

    *library = tmp_library;

    return 0;
}

int main(void) {
    int impl_number = 1;

    void *library = NULL;

    if (set_mathematics_impl(impl_number, &library)) {
        return 1;
    }

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
        if (strcmp(type, "0") == 0) {
            impl_number = impl_number == 1 ? 2 : 1;

            if (set_mathematics_impl(impl_number, &library)) {
                dlclose(library);

                return 1;
            }

            sprintf(message, "Impl successfully changed!\n");
        } else if (strcmp(type, "1") == 0) {
            ProcessCommand1(prime_count_fn, message);
        } else if (strcmp(type, "2") == 0) {
            ProcessCommand2(pi_fn, message);
        } else {
            sprintf(message, "Invalid command!\n");
        }

        write(STDOUT_FILENO, message, strlen(message));

        memset(command, 0, MAX_BUFFER_SIZE);
    }

    dlclose(library);

    return 0;
}
