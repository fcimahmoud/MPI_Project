#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

// Direction constants
#define ASCENDING 1
#define DESCENDING 0

// Helper: merge two halves based on direction
void bitonicMerge(vector<int>& localData, int partnerData[], int localSize, int direction, bool keepLower) {
    vector<int> merged(localSize * 2);
    copy(localData.begin(), localData.end(), merged.begin());
    copy(partnerData, partnerData + localSize, merged.begin() + localSize);

    sort(merged.begin(), merged.end());
    if ((direction == ASCENDING && keepLower) || (direction == DESCENDING && !keepLower)) {
        copy(merged.begin(), merged.begin() + localSize, localData.begin());
    } else {
        copy(merged.end() - localSize, merged.end(), localData.begin());
    }
}

void runBitonicSort(int rank, int size) {
    vector<int> data;
    int totalSize = 0;

    if (rank == 0) {
        cout << "------------------------------\n";
        cout << "Bitonic Sort Selected\n";
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

        if ((totalSize % size) != 0) {
            cerr << "Error: Data size must be divisible by number of processes for Bitonic Sort.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if ((size & (size - 1)) != 0) {
            cerr << "Error: Number of processes must be a power of 2 for Bitonic Sort.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        cout << "Reading data from file... Total elements: " << totalSize << endl;
    }

    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int chunkSize = totalSize / size;

    vector<int> localData(chunkSize);
    MPI_Scatter(data.data(), chunkSize, MPI_INT, localData.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Local sort
    sort(localData.begin(), localData.end());

    // Bitonic Sort across processes
    int stages = static_cast<int>(log2(size));

    for (int stage = 0; stage < stages; ++stage) {
        for (int step = stage; step >= 0; --step) {
            int partner = rank ^ (1 << step);

            int direction = ((rank >> (stage + 1)) % 2 == 0) ? ASCENDING : DESCENDING;
            bool keepLower = ((rank >> step) % 2 == 0);

            vector<int> partnerData(chunkSize);
            MPI_Sendrecv(localData.data(), chunkSize, MPI_INT, partner, 0,
                            partnerData.data(), chunkSize, MPI_INT, partner, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            bitonicMerge(localData, partnerData.data(), chunkSize, direction, keepLower);
        }
    }

    // Gather final sorted array
    vector<int> sortedData;
    if (rank == 0) sortedData.resize(totalSize);

    MPI_Gather(localData.data(), chunkSize, MPI_INT,
                sortedData.data(), chunkSize, MPI_INT,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Sorted Data: ";
        for (auto &&i : sortedData)
        {
            cout << i << " ";
        }cout << endl;
        

        ofstream outFile("output_bitonicsort.txt");
        for (int num : sortedData) {
            outFile << num << " ";
        }
        outFile.close();
        cout << "Result: Sorted Array is stored at file output_bitonicsort.txt\n";
    }
}
