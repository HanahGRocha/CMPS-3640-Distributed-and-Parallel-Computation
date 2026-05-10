/*! \file pi.cpp
 * \brief Program to calculate value of pi using integral.
 * Using for lab6, modified from original
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


int main(int argc, char* argv[])
{
    long i;
    double x, pi, sum = 0.0; // shared global sum (threads add local sums atomically)
    double start_time, run_time;
    step = 1.0 / (double)num_steps;
    int numThreads = 1;
    if (argc > 1) numThreads = atoi(argv[1]);
    if (numThreads > 0) omp_set_num_threads(numThreads);
    #pragma omp parallel shared (sum, start_time)
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        // single: executed by one thread only. Use nowait so other threads
        #pragma omp single
        {
            printf("Running with %d threads\n", nthreads);
            start_time = omp_get_wtime(); 
            printf("Thread %d of %d is starting the computation.\n", tid, nthreads);
        }
        long chunk = num_steps / nthreads;
        long start = tid * chunk;
        long end = (tid == nthreads - 1) ? num_steps : start + chunk;
        double local_sum = 0.0;

        //looping over subintervals to get midpoint and area
        for (i = start; i < end; i++)
        {
            x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }
        #pragma omp atomic
        sum += local_sum;
    //rectangle width is multipled 
    }// end parallel region -> implicit barrier here ensures all threads finish
    pi = step * sum;
    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %ld steps is %lf in %lf seconds\n", num_steps, pi, run_time);
    return 0;
}
