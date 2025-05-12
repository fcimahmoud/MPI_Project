#include <iostream>
#include <vector>
#include <algorithm>

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

int main() {
    vector<int> data = {23, 10, 7, 9, 18, 2, 1, 5};

    // Make sure size is a power of 2
    int n = data.size();
    if ((n & (n - 1)) != 0) {
        cout << "Array size must be a power of 2.\n";
        return 1;
    }

    cout << "Original array:\n";
    for (int num : data) cout << num << " ";
    cout << endl;

    bitonicSort(data, 0, n, true);  // true for ascending

    cout << "Sorted array:\n";
    for (int num : data) cout << num << " ";
    cout << endl;

    return 0;
}
