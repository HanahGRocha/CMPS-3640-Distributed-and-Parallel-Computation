#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

using namespace std;

int main() {
    const int N = 100000;
    vector <int> data(N);
    vector <int> hist(10,0); // Bins (0-9)
    //Locks
    omp_lock_t locks[10];
    //init locks
    for (int i = 0; i < 10; i++){
        omp_init_lock(&locks[i]);
    }

    //seed rand num generator
    srand(time(0));

    //Fill array with rand nums 0-9
    for (int i = 0; i < N; i++){
        data[i] = rand() % 10;
    }

    cout << "Max threads available: " << omp_get_max_threads() << endl;

    #pragma omp parallel 
    {
        #pragma omp single
        {
            cout << "Threads being used: " << omp_get_num_threads() << endl;
        }
    }

    //start timing
    auto start = chrono::high_resolution_clock::now();

    //parallize histogram
    #pragma omp parallel for 
    //compute histogram
    for (int i = 0; i < N; i++){
        int value = data[i];
        omp_set_lock(&locks[value]); //lock bin
        hist[value]++;
        omp_unset_lock(&locks[value]); //unlock bin
    }

    //end time
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    //Destroy locks
    for (int i = 0; i < 10; i++){
        omp_destroy_lock(&locks[i]);
    }

    //print histogram
    cout << "Histogram:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << "Bin " << i << ": " << hist[i] << endl;
    }

    //Print time
    cout << "\nTime Taken(Parallel): " << duration.count() << " seconds\n" << endl;
    return 0;
}