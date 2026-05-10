/*! \file HelloWorldOMP.cpp
 * \brief Hello World program in OpenMP.
 */
#include <omp.h>
#include <stdio.h>
int main()
{
    omp_set_num_threads(6);
#pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf(" Hello(%d)", ID);
        printf(" World(%d)\n", ID);
    }
    return 0;
}
