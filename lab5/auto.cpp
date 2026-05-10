/*! \file pi.cpp
 * \brief Program to calculate value of pi using integral.
 *
 * This program will numerically compute the integral of
 *
 * 4/(1+x*x)
 *
 * from 0 to 1. The value of this integral is pi -- which
 * is great since it gives us an easy way to check the answer.
 *
 * The is the original sequential program. It uses the timer
 * from the OpenMP runtime library.
 *
 * History: Written by Tim Mattson, 11/99.
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
//num_steps is the number of rectangles 
static long num_steps = 1000000;
//more steps means better accuracy, but more work
double step;
#define MAX_THREADS 128

int main(int argc, char* argv[])
{
    long i;
    int numThreads = 1;
    double x, pi, sum = 0.0;
    double start_time, run_time;

    //command line usage handling
    if (argc > 1)
        numThreads = atoi(argv[1]);
    if (numThreads <= 0)
        numThreads = 1;

    if (argc > 2) {
        long ns = atol(argv[2]);
        if (ns > 0) num_steps = ns;
    }

    if (numThreads > MAX_THREADS) {
        printf("ERROR: numThread=%d exceeds MAX_THREADS=%d\n", numThreads, MAX_THREADS);
        return 1;
    }
    omp_set_num_threads(numThreads);
    omp_set_schedule(omp_sched_static, 1000);
    printf("schedule=auto\n");
    step = 1.0 / (double)num_steps;
    start_time = omp_get_wtime(); // this is the timer we want to use when using OpemMP
                                  //looping over subintervals to get midpoint and area
#pragma omp parallel for reduction(+:sum) schedule(auto)
    for (i = 0; i < num_steps; i++)
    {
        x = (i + 0.5) * step;
        sum = sum + 4.0 / (1.0 + x * x);
    }
    //rectangle width is multipled 
    pi = step * sum;
    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi,
            run_time);
    return 0;
}
