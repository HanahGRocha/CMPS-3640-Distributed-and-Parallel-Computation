#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

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

        #pragma omp parallel for reduction(+:sum)
        for (long i = 0; i < N; i++) {
            sum += A[i];
        }

        double ave = sum / (double)N;
        double time = omp_get_wtime() - start;

        printf("PARALLEL threads=%d  N=%ld  ave=%f  time=%lf sec\n",
               numThreads, N, ave, time);

        free(A);
    }

    return 0;
}

