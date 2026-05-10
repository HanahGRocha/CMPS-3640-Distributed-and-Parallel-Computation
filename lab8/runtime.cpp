#include <omp.h>
void main()
{
int num_threads;
omp_set_dynamic(0); // turn off dynamic mode
omp_set_num_threads(omp_get_num_procs()); // request threads = # of
processors
#pragma omp parallel
{
int id = omp_get_thread_num(); // get the thread you are in
#pragma omp single
num_threads = omp_get_num_threads(); // how many threads are there?
do_lots_of_stuff(id); // whatever function you want
}
}
