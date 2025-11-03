#ifndef MAI_OS_2025_METRICS_H
#define MAI_OS_2025_METRICS_H

#include <lab2/geometry.h>


double Acceleration(double t_s,
                    double t_p);

double Efficiency(double s,
                  uint64_t p);

double Measure(Points points,
               uint64_t threads_count,
               Triangle expected);

#endif //MAI_OS_2025_METRICS_H
