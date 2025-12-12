#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lab4/commands.h>


int ParseInt(char *string,
             int *number) {
    if (!string) {
        return 1;
    }

    char *end;

    long tmp = strtol(string, &end, 10);

    if (tmp < INT_MIN || tmp > INT_MAX) {
        return 1;
    }

    if (*end != '\0') {
        return 1;
    }

    *number = (int) tmp;

    return 0;
}

int ProcessCommand1(PrimeCountFn prime_count_fn,
                    char *message) {
    int a = 0, b = 0;

    if (ParseInt(strtok(NULL, DELIMITERS), &a)) {
        sprintf(message, "Can`t parse number!\n");

        return 0;
    }

    if (ParseInt(strtok(NULL, DELIMITERS), &b)) {
        sprintf(message, "Can`t parse number!\n");

        return 0;
    }

    if (strtok(NULL, DELIMITERS) != NULL) {
        sprintf(message, "Unknown arguments!\n");

        return 0;
    }

    int prime_result = prime_count_fn(a, b);

    if (prime_result == -1) {
        sprintf(message, "Invalid number range!\n");
    } else {
        sprintf(message, "Primes count in [a, b]: %d\n", prime_result);
    }

    return 0;
}

int ProcessCommand2(PiFn pi_fn,
                    char *message) {
    int k = 0;

    if (ParseInt(strtok(NULL, DELIMITERS), &k)) {
        sprintf(message, "Can`t parse number!\n");

        return 0;
    }

    if (strtok(NULL, DELIMITERS) != NULL) {
        sprintf(message, "Unknown arguments!\n");

        return 0;
    }

    float pi_result = pi_fn(k);

    if (pi_result + 1.f < FLT_EPSILON) {
        sprintf(message, "Invalid number of series!\n");
    } else {
        sprintf(message, "Pi by series: %f\n", pi_result);
    }

    return 0;
}
