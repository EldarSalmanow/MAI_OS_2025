#ifndef MAI_OS_2025_MATHEMATICS_H
#define MAI_OS_2025_MATHEMATICS_H

typedef int (*PrimeCountFn)(int a,
                            int b);

typedef float (*PiFn)(int k);

int prime_count(int a,
                int b);

float pi(int k);

#endif //MAI_OS_2025_MATHEMATICS_H