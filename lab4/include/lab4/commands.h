#ifndef MAI_OS_2025_COMMANDS_H
#define MAI_OS_2025_COMMANDS_H

#include <lab4/mathematics.h>

#define EXIT_COMMAND "exit\n"
#define DELIMITERS   " \t\n"


int ProcessCommand1(PrimeCountFn prime_count_fn,
                    char *message);

int ProcessCommand2(PiFn pi_fn,
                    char *message);

#endif //MAI_OS_2025_COMMANDS_H