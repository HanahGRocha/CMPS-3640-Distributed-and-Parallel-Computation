//Name: Hanah Rocha
//Date: 3/5/2026
// Lab 6
// Reference code from lab 5 of parallel array code using reduction
//Description: This program calculates the average of an array of floats 
// using OpenMP for parallelization. 
// It reports progress during the computation and 
// measures execution time for different array sizes and thread counts.

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for usleep

int main(int argc, char* argv[]) {
    long sizes[] = {1000, 10000, 100000, 1000000, 10000000, 100000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    int numThreads = 1;

    if (argc > 1) numThreads = atoi(argv[1]);
    if (numThreads <= 0) numThreads = 1;

    omp_set_num_threads(numThreads);

    for (int s = 0; s < num_sizes; s++) {
        long N = sizes[s];

        float *A = (float*) malloc(sizeof(float) * N);
        if (A == NULL) {
            printf("Failed to allocate array for N=%ld\n", N);
            continue;
        }

        for (long i = 0; i < N; i++) {
            A[i] = (float)(i % 1000) * 0.001;
        }

        double start = omp_get_wtime();
        double sum = 0.0;
        long progress = 0;

    #pragma omp parallel shared(progress, A, N)
        {
            int tid = omp_get_thread_num();
            int nthreads = omp_get_num_threads();
            // Identify threads at start
            #pragma omp critical
            {
                if (tid == 0)
                    printf("Thread %d is MASTER\n", tid);
                else
                    printf("Thread %d is WORKER\n", tid);
            }
            if (tid == 0) {
                // Master thread: poll and print progress until workers complete
                long last_report = -1;
                while (1) {
                    long cur;
                    #pragma omp atomic read
                    cur = progress;

                    if (cur != last_report) {
                        printf("[MASTER %d] Progress: %ld / %ld (%.2f%%)\n",
                            tid, cur, N,
                            100.0 * (double)cur / (double)N);
                        last_report = cur;
                    }
                    if (cur >= N) break; // finished
                    usleep(200000); // sleep 200 ms to avoid busy-waiting
                }
                printf("[MASTER %d] Finished tracking.\n", tid);

            } else {
                // Worker threads: perform the summation. Use dynamic scheduling.
                // We use a small local counter and batch updates to progress to reduce atomic overhead.
                const int BATCH = 128; // adjust: larger for fewer atomics, but progress updates
                long local_count = 0;
                
                printf("[WORKER %d] Starting work.\n", tid);

                #pragma omp for schedule(dynamic) reduction(+:sum) nowait
                for (long i = 0; i < N; i++) {
                    sum += A[i];
                    local_count++;

                    if (local_count >= BATCH) {
                        #pragma omp atomic
                        progress += local_count;
                        local_count = 0;
                    }
                }

                // flush remaining local_count
                if (local_count > 0) {
                    #pragma omp atomic
                    progress += local_count;
                }
                printf("[WORKER %d] Finished work.\n", tid);
            }
        } // end parallel

        double time = omp_get_wtime() - start;
        double ave = sum / (double)N;

        printf("RESULT threads=%d  N=%ld  ave=%f  time=%lf sec\n\n",
               numThreads, N, ave, time);

        free(A);
    }

    return 0;
}

