#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <lab2/geometry.h>

#define EPSILON 1e-9

Point CreatePoint(double x,
                  double y,
                  double z) {
    Point point;
    point.x = x;
    point.y = y;
    point.z = z;

    return point;
}

Triangle CreateTriangle(Point a,
                        Point b,
                        Point c,
                        double area) {
    Triangle triangle;
    triangle.a = a;
    triangle.b = b;
    triangle.c = c;
    triangle.area = area;

    return triangle;
}

double TriangleArea(Point a,
                    Point b,
                    Point c) {
    double ab_x = b.x - a.x;
    double ab_y = b.y - a.y;
    double ab_z = b.z - a.z;

    double ac_x = c.x - a.x;
    double ac_y = c.y - a.y;
    double ac_z = c.z - a.z;

    double cross_x = ab_y * ac_z - ab_z * ac_y;
    double cross_y = ab_z * ac_x - ab_x * ac_z;
    double cross_z = ab_x * ac_y - ab_y * ac_x;

    double cross_length = sqrt(cross_x * cross_x + cross_y * cross_y + cross_z * cross_z);

    return 0.5 * cross_length;
}

Triangle MaxTriangleLocal(Points points,
                          uint64_t start,
                          uint64_t end) {
    double local_max_area = 0.0;
    uint64_t max_a = 0, max_b = 0, max_c = 0;

    for (uint64_t i = start; i < end; ++i) {
        for (uint64_t j = i + 1; j < points.size; ++j) {
            for (uint64_t k = j + 1; k < points.size; ++k) {
                double triangle_area = TriangleArea(points.data[i],
                                                    points.data[j],
                                                    points.data[k]);

                if (triangle_area > local_max_area) {
                    local_max_area = triangle_area;
                    max_a = i;
                    max_b = j;
                    max_c = k;
                }
            }
        }
    }

    return CreateTriangle(points.data[max_a],
                          points.data[max_b],
                          points.data[max_c],
                          local_max_area);
}

typedef struct {
    Points points;

    uint64_t start, end;

    Triangle result;
} MaxTriangleLocalArgs;

void *MaxTriangleLocalThread(void *args) {
    MaxTriangleLocalArgs *arguments = (MaxTriangleLocalArgs *) args;

    arguments->result = MaxTriangleLocal(arguments->points,
                                         arguments->start,
                                         arguments->end);

    return NULL;
}

void JoinThreads(pthread_t *threads,
                 uint64_t threads_count) {
    for (uint64_t i = 0; i < threads_count; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            const char message[] = "Can`t join thread!\n";

            write(STDOUT_FILENO, message, sizeof(message));
        }
    }
}

Triangle MaxTriangle(Points points,
                     uint64_t threads_count) {
    if (points.size < 3) {
        free(points.data);

        const char message[] = "Minimum count of points for calculate maximum triangle area is 3!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        exit(EXIT_FAILURE);
    }

    if (threads_count == 0) {
        return MaxTriangleLocal(points, 0, points.size);
    }

    uint64_t points_per_thread = points.size / threads_count;

    MaxTriangleLocalArgs *arguments = (MaxTriangleLocalArgs *) malloc(threads_count * sizeof(MaxTriangleLocalArgs));

    if (!arguments) {
        free(points.data);

        const char message[] = "Can`t allocate memory for arguments!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        exit(EXIT_FAILURE);
    }

    pthread_t *threads = (pthread_t *) malloc(threads_count * sizeof(pthread_t));

    if (!threads) {
        free(points.data);

        const char message[] = "Can`t allocate memory for threads!\n";

        write(STDOUT_FILENO, message, sizeof(message));

        exit(EXIT_FAILURE);
    }

    for (uint64_t i = 0, start = 0; i < threads_count; ++i, start += points_per_thread) {
        arguments[i].points = points;
        arguments[i].start = start;
        arguments[i].end = start + points_per_thread;

        if (i + 1 == threads_count) {
            arguments[i].end = points.size;
        }

        if (pthread_create(&threads[i], NULL, MaxTriangleLocalThread, &arguments[i]) != 0) {
            const char message_t[] = "Can`t create thread!\n";
            write(STDOUT_FILENO, message_t, sizeof(message_t));

            JoinThreads(threads, i);

            free(threads);
            free(arguments);

            free(points.data);

            exit(EXIT_FAILURE);
        }
    }

    bool joined = true;
    for (uint64_t i = 0; i < threads_count; ++i) {
        if (pthread_join(threads[i], NULL) != 0) {
            const char message[] = "Can`t join thread!\n";
            write(STDOUT_FILENO, message, sizeof(message));

            joined = false;
        }
    }

    free(threads);

    if (!joined) {
        free(arguments);
        free(points.data);

        exit(EXIT_FAILURE);
    }

    Triangle max_triangle = arguments[0].result;
    for (uint64_t i = 1; i < threads_count; ++i) {
        Triangle local_triangle = arguments[i].result;

        if (local_triangle.area - max_triangle.area > EPSILON) {
            max_triangle = local_triangle;
        }
    }

    free(arguments);

    return max_triangle;
}
