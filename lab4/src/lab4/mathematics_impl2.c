#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lab4/mathematics.h>


int prime_count(int a,
                int b) {
    if (a < 1 || b < 1
     || a >= b) {
        return -1;
     }

    bool *primes = (bool *) malloc((b + 1) * sizeof(bool));

    if (!primes) {
        return 0;
    }

    memset(primes, true, (b + 1) * sizeof(bool));

    for (int i = 2; i <= b + 1; i++) {
        if (!primes[i]) {
            continue;
        }

        for (int j = 2 * i; j <= b + 1; j += i) {
            primes[j] = false;
        }
    }

    int primes_count = 0;
    for (int i = a; i <= b; ++i) {
        if (primes[i]) {
            ++primes_count;
        }
    }

    free(primes);

    return primes_count;
}

float pi(int k) {
    if (k < 1) {
        return -1.f;
    }

    double result = 4.0 / 3.0;

    for (int n = 2; n <= k; ++n) {
        double n2 = 2.0 * (double) n;
        double up = n2 * n2;
        double down = (n2 - 1.0) * (n2 + 1.0);

        result *= up / down;
    }

    return 2.f * (float) result;
}
