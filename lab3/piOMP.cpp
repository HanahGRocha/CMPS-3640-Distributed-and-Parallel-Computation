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
static long num_steps = 100000000;
//more steps means better accuracy, but more work
double step;
#define MAX_THREADS 128

int main(int argc, char* argv[])
{
    int numThreads = 4, actThreads = 0;
    int i;
    double x, pi, sum = 0.0;
    double start_time, run_time;
   
    //command line usage handling
    if (argc > 1)
        numThreads = atoi(argv[1]);
    if (numThreads <= 0)
        numThreads = 1;

    if (argc > 2) {
        long ns = atoi(argv[2]);
        if (ns > 0) num_steps = ns;
    } 

    if (numThreads > MAX_THREADS) {
        printf("ERROR: numThread=%d exceeds MAX_THREADS=%d\n", numThreads, MAX_THREADS);
    }

    //Per-thread partial sums, no race-condition 
    step = 1.0 / (double)num_steps;
    double cumsum[MAX_THREADS];
    for (int k = 0; k < MAX_THREADS; k++){
        cumsum[k] = 0.0;
    }
    omp_set_num_threads(numThreads);
    start_time = omp_get_wtime(); // this is the timer we want to use when using OpemMP
    #pragma omp parallel
    {
        int id, NT;
        id = omp_get_thread_num();
        NT = omp_get_num_threads();
        if (id == 0)
            actThreads = NT;
        double local = 0.0;

        //Approach a.
        for (long i = id; i < num_steps; i += NT) {
            double x = (i + 0.5) * step;
            local += 4.0 / (1 + x * x);
        }
        cumsum[id] = local;
    }
    
    //combine sums
    sum = 0.0;
    for (int i = 0; i < actThreads; i++) {
        sum += cumsum[i]; 
    }

    //rectangle width is multipled 
    pi = step * sum;
    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi,
            run_time);
    printf("threads requested=%d actual=%d\n", numThreads, actThreads);
    return 0;
}
