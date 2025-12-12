#include <stdbool.h>

#include <lab4/mathematics.h>


int prime_count(int a,
                int b) {
    if (a < 1 || b < 1
     || a >= b) {
        return -1;
    }

    int primes_count = 0;

    for (int i = a; i <= b; ++i) {
        bool prime = true;

        for (int j = 2; j < i; ++j) {
            if (i % j == 0) {
                prime = false;

                break;
            }
        }

        if (prime) {
            ++primes_count;
        }
    }

    return primes_count;
}

float pi(int k) {
    if (k <= 0) {
        return -1.f;
    }

    double result = 0.f;

    for (int n = 0, sign = 1; n < k; ++n, sign = -sign) {
        result += (double) sign / (double) (2 * n + 1);
    }

    return 4.f * (float) result;
}
