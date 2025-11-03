#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <lab2/metrics.h>


double Acceleration(double t_s,
                    double t_p) {
    return t_s / t_p;
}

double Efficiency(double s,
                  uint64_t p) {
    return s / (double) p;
}

double Measure(Points points,
               uint64_t threads_count,
               Triangle expected) {
    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        free(points.data);

        const char message[] = "Can`t get start time!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        exit(EXIT_FAILURE);
    }

    Triangle calculated = MaxTriangle(points, threads_count);

    if (calculated.area != expected.area) {
        const char message[] = "[ERROR] Invalid value of calculation maximum area!\n";

        write(STDOUT_FILENO, message, sizeof(message));
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        free(points.data);

        const char message[] = "Can`t get end time!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        exit(EXIT_FAILURE);
    }

    return (double) (end.tv_sec - start.tv_sec) * 1000.0 + (double) (end.tv_nsec - start.tv_nsec) / 1000000.0;
}
