#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>

using namespace std;

int main() {
    const int N = 100000;
    vector <int> data(N);
    vector <int> hist(10,0); // Bins (0-9)

    //seed rand num generator
    srand(time(0));

    //Fill array with rand nums 0-9
    for (int i = 0; i < N; i++){
        data[i] = rand() % 10;
    }

    //start timing
    auto start = chrono::high_resolution_clock::now();

    //compute histogram
    for (int i = 0; i < N; i++){
        int value = data[i];
        hist[value]++;
    }

    //end time
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    //print histogram
    cout << "Histogram:" << endl;
    for (int i = 0; i < 10; i++) {
        cout << "Bin " << i << ": " << hist[i] << endl;
    }

    //Print time
    cout << "\nTime Taken(Sequential): " << duration.count() << " seconds\n" << endl;
    return 0;
}