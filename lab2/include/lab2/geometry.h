#ifndef MAI_OS_2025_GEOMETRY_H
#define MAI_OS_2025_GEOMETRY_H

#include <stdint.h>

typedef struct {
    double x, y, z;
} Point;

Point CreatePoint(double x,
                  double y,
                  double z);

typedef struct {
    Point *data;
    uint64_t size;
} Points;

typedef struct {
    Point a, b, c;

    double area;
} Triangle;

Triangle CreateTriangle(Point a,
                        Point b,
                        Point c,
                        double area);

double TriangleArea(Point a,
                    Point b,
                    Point c);

Triangle MaxTriangle(Points points,
                     uint64_t threads_count);

#endif //MAI_OS_2025_GEOMETRY_H
