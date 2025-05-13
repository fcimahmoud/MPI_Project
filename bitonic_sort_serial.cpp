#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "mpi.h"
using namespace std;

// Swap two elements depending on the direction
void compareAndSwap(vector<int>& arr, int i, int j, bool ascending) {
    if (ascending == (arr[i] > arr[j])) {
        swap(arr[i], arr[j]);
    }
}

// Merges bitonic sequences into sorted sequence
void bitonicMerge(vector<int>& arr, int low, int count, bool ascending) {
    if (count > 1) {
        int k = count / 2;
        for (int i = low; i < low + k; i++) {
            compareAndSwap(arr, i, i + k, ascending);
        }
        bitonicMerge(arr, low, k, ascending);
        bitonicMerge(arr, low + k, k, ascending);
    }
}

// Recursively builds a bitonic sequence and sorts it
void bitonicSort(vector<int>& arr, int low, int count, bool ascending) {
    if (count > 1) {
        int k = count / 2;

        // First half in ascending order
        bitonicSort(arr, low, k, true);

        // Second half in descending order
        bitonicSort(arr, low + k, k, false);

        // Merge the entire sequence in the desired direction
        bitonicMerge(arr, low, count, ascending);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    vector<int> data;

        cout << "Please enter the path to the input file: ";
        string filePath;
        cin >> filePath;

        ifstream file(filePath);
        int num;
        while (file >> num) {
            data.push_back(num);
        }

    double startTime = 0.0, endTime = 0.0;
    // C++ Function to get time
    startTime = MPI_Wtime();

    // Make sure size is a power of 2
    int n = data.size();
    if ((n & (n - 1)) != 0) {
        cout << "Array size must be a power of 2.\n";
        return 1;
    }

    // cout << "Original array:\n";
    // for (int num : data) cout << num << " ";
    // cout << endl;

    bitonicSort(data, 0, n, true);  // true for ascending

    endTime = MPI_Wtime();
    double elapsed = endTime - startTime;
    cout << "Execution Time: " << elapsed << " seconds\n";
    cout << "------------------------------\n";
    cout << "Sorted array:\n";
    //for (int num : data) cout << num << " ";
    cout << endl;

    MPI_Finalize();
    return 0;
}
