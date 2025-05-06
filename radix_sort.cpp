#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>

using namespace std;

int getDigit(int number, int digitPos) {
    return (number / static_cast<int>(pow(10, digitPos))) % 10;
}

void radixSort(vector<int>& arr) {
    if (arr.empty()) return;
    int maxVal = *max_element(arr.begin(), arr.end());
    int maxDigits = (maxVal == 0) ? 1 : static_cast<int>(log10(maxVal)) + 1;

    for (int digit = 0; digit < maxDigits; ++digit) {
        vector<vector<int>> buckets(10);
        for (int num : arr) {
            int d = getDigit(num, digit);
            buckets[d].push_back(num);
        }
        arr.clear();
        for (int d = 0; d < 10; ++d)
            arr.insert(arr.end(), buckets[d].begin(), buckets[d].end());
    }
}

void runRadixSort(int rank, int size) {
    vector<int> data;
    int totalSize = 0;

    if (rank == 0) {
        cout << "------------------------------\n";
        cout << "Radix Sort Selected\n";
        cout << "------------------------------\n";
        cout << "Please enter the path to the input file: ";
        string filePath;
        cin >> filePath;

        ifstream file(filePath);
        int num;
        while (file >> num) {
            data.push_back(num);
        }

        totalSize = static_cast<int>(data.size());
        cout << "Reading data from file... Total elements: " << totalSize << endl;
    }

    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate send counts and displacements
    int baseSize = totalSize / size;
    int remainder = totalSize % size;
    vector<int> counts(size, baseSize);
    for (int i = 0; i < remainder; ++i) counts[i]++;
    vector<int> displs(size, 0);
    for (int i = 1; i < size; ++i)
        displs[i] = displs[i - 1] + counts[i - 1];

    vector<int> localData(counts[rank]);
    MPI_Scatterv(data.data(), counts.data(), displs.data(), MPI_INT,
                    localData.data(), counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    // Local radix sort
    radixSort(localData);

    // Gather sorted sizes
    vector<int> recvCounts(size);
    MPI_Gather(&counts[rank], 1, MPI_INT, recvCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> recvDispls(size, 0);
    if (rank == 0) {
        for (int i = 1; i < size; ++i)
            recvDispls[i] = recvDispls[i - 1] + recvCounts[i - 1];
    }

    vector<int> sortedData;
    if (rank == 0) sortedData.resize(totalSize);

    MPI_Gatherv(localData.data(), counts[rank], MPI_INT,
                sortedData.data(), recvCounts.data(), recvDispls.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Merge sorted chunks
        vector<int> finalSorted;
        for (int i = 0; i < size; ++i) {
            vector<int> chunk(sortedData.begin() + recvDispls[i], 
                                sortedData.begin() + recvDispls[i] + recvCounts[i]);
            finalSorted.insert(finalSorted.end(), chunk.begin(), chunk.end());
        }
        sort(finalSorted.begin(), finalSorted.end());

        for (auto &&i : finalSorted)
        {
            cout << i << " ";
        }cout << endl;

        ofstream outFile("output_radixsort.txt");
        for (int num : finalSorted)
            outFile << num << " ";
        outFile.close();
        cout << "Result: Sorted Array is stored at file output_radixsort.txt\n";
    }
}
