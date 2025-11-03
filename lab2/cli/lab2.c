#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lab2/geometry.h>
#include <lab2/metrics.h>

#define POINTS_COUNT 1000
#define MAX_BUFFER_SIZE 1024

int main(void) {
    const uint64_t Threads[] = {
            1, 2, 4, 16, 128, 256, 512, 1024
    };

    const uint64_t ThreadsCount = sizeof(Threads) / sizeof(Threads[0]);

    Points points;
    points.data = (Point *) malloc(POINTS_COUNT * sizeof(Point));

    if (!points.data) {
        const char message[] = "Can`t allocate points!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        return 1;
    }

    points.size = POINTS_COUNT;

    for (uint64_t i = 0; i < points.size; ++i) {
        points.data[i] = CreatePoint(rand() % 1000,
                                     rand() % 1000,
                                     rand() % 1000);
    }

    Triangle expected = MaxTriangle(points, 0);

    double linear = Measure(points,
                            Threads[0],
                            expected);

    char buffer[MAX_BUFFER_SIZE + 1] = {0};
    sprintf(buffer,
            "%.9f %.9f %.9f %lu\n",
            linear,
            1.0,
            1.0,
            Threads[0]);
    write(STDOUT_FILENO, buffer, strlen(buffer));

    for (uint64_t i = 1; i < ThreadsCount; ++i) {
        double total = Measure(points,
                               Threads[i],
                               expected);
        double acceleration = Acceleration(linear,
                                           total);
        double efficiency = Efficiency(acceleration,
                                       Threads[i]);

        sprintf(buffer,
                "%.9f %.9f %.9f %lu\n",
                total,
                acceleration,
                efficiency,
                Threads[i]);
        write(STDOUT_FILENO, buffer, strlen(buffer));
    }

    free(points.data);

    return 0;
}
