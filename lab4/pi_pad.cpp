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


int main(int argc, char* argv[])
{
    double start_time, run_time, pi;
    pi = 0.0;
    const int PAD = 8;
    const int MAX_THREADS = 128;
    static double sum[MAX_THREADS][PAD];
    int numThreads = 4;

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


    for (int k = 0; k < MAX_THREADS; k++){
        for (int j = 0; j < PAD; j++)
            sum[k][j] = 0.0;
    }    
    omp_set_num_threads(numThreads);
    step = 1.0 / (double)num_steps;
    start_time = omp_get_wtime(); // this is the timer we want to use when using OpemMP
    int usedThreads = 0;
    
    //looping over subintervals to get midpoint and area
    #pragma omp parallel
    {
        int id = omp_get_thread_num();
        int nt = omp_get_num_threads();
        #pragma omp single 
        usedThreads = nt;
        printf("Using %d threads\n", omp_get_num_threads());
        for (long i = id; i < num_steps; i+=nt)
        {
            double x = (i + 0.5) * step;
            sum[id][0] += 4.0 / (1.0 + x * x);
        }
    }
    double total = 0.0;
    for (int x = 0; x < usedThreads; x++)
        total += sum[x][0]; 
        
    pi = step * total;
    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi,
            run_time);
    return 0;
}
