#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <map>

using namespace std;

int getDigit(int number, int digitPos) {
    return (number / static_cast<int>(pow(10, digitPos))) % 10;
}

void runRadixSort(int rank, int size) {
    vector<int> data;
    int totalSize = 0;

    double startTime = 0.0, endTime = 0.0;

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

        totalSize = data.size();
        cout << "Reading data from file... Total elements: " << totalSize << endl;
    }

    // Start timing after broadcasting input and before processing
    MPI_Barrier(MPI_COMM_WORLD);  // Ensure all processes are synced before timing
    startTime = MPI_Wtime();

    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Distribute data initially
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

    // Determine number of digits
    int localMax = localData.empty() ? 0 : *max_element(localData.begin(), localData.end());
    int globalMax;
    MPI_Allreduce(&localMax, &globalMax, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    int maxDigits = (globalMax == 0) ? 1 : static_cast<int>(log10(globalMax)) + 1;

    for (int digit = 0; digit < maxDigits; ++digit) {

        // 1. Local bucket counts
        vector<vector<int>> localBuckets(10);
        for (int num : localData) {
            int d = getDigit(num, digit);
            localBuckets[d].push_back(num);
        }

        // 2. Local counts per bucket
        vector<int> localBucketSizes(10);
        for (int i = 0; i < 10; ++i)
            localBucketSizes[i] = localBuckets[i].size();

        // 3. Gather all sizes to rank 0
        vector<int> globalBucketSizes(10 * size);
        MPI_Gather(localBucketSizes.data(), 10, MPI_INT,
                    globalBucketSizes.data(), 10, MPI_INT,
                    0, MPI_COMM_WORLD);

        vector<int> sendCounts(size), sendDispls(size);
        vector<int> flattened;

        if (rank == 0) {
            // 4. Rank 0 combines all buckets by digit
            vector<vector<int>> finalBuckets(10);
            for (int p = 0; p < size; ++p) {
                for (int d = 0; d < 10; ++d) {
                    int count = globalBucketSizes[p * 10 + d];
                    if (count == 0) continue;

                    // Receive data chunk from each process
                    vector<int> recvBuffer(count);
                    if (p == 0) {
                        // If it's rank 0, we already have local data
                        recvBuffer = localBuckets[d];
                    } else {
                        MPI_Recv(recvBuffer.data(), count, MPI_INT, p, d, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    }
                    finalBuckets[d].insert(finalBuckets[d].end(), recvBuffer.begin(), recvBuffer.end());
                }
            }

            // Flatten the data
            for (int i = 0; i < 10; ++i) {
                flattened.insert(flattened.end(), finalBuckets[i].begin(), finalBuckets[i].end());
            }
            

            // Split again into new counts for redistribution
            int newTotal = flattened.size();
            baseSize = newTotal / size;
            remainder = newTotal % size;
            for (int i = 0; i < size; ++i)
                sendCounts[i] = baseSize + (i < remainder ? 1 : 0);
            sendDispls[0] = 0;
            for (int i = 1; i < size; ++i)
                sendDispls[i] = sendDispls[i - 1] + sendCounts[i - 1];
        } else {
            // 5. Send local buckets to rank 0
            for (int d = 0; d < 10; ++d) {
                if (!localBuckets[d].empty()) {
                    MPI_Send(localBuckets[d].data(), localBuckets[d].size(), MPI_INT, 0, d, MPI_COMM_WORLD);
                }
            }
        }

        // Broadcast sendCounts
        MPI_Bcast(sendCounts.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(sendDispls.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

        // Resize localData for next round
        localData.resize(sendCounts[rank]);
        MPI_Scatterv(flattened.data(), sendCounts.data(), sendDispls.data(), MPI_INT,
                        localData.data(), sendCounts[rank], MPI_INT, 0, MPI_COMM_WORLD);
    }


    vector<int> finalCounts(size);
    int localSize = localData.size();
    MPI_Gather(&localSize, 1, MPI_INT, finalCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> finalDispls(size);
    vector<int> finalSorted(totalSize);
    
    if (rank == 0) {
        finalDispls[0] = 0;
        for (int i = 1; i < size; ++i)
        finalDispls[i] = finalDispls[i - 1] + finalCounts[i - 1];
    }

    MPI_Gatherv(localData.data(), localSize, MPI_INT,
                finalSorted.data(), finalCounts.data(), finalDispls.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    // End timing after computation and reduction
    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    if (rank == 0) {
        double elapsed = endTime - startTime;
        cout << "Execution Time with " << size << " process(es): " << elapsed << " seconds\n";
        cout << "------------------------------\n";

        ofstream outFile("output_radixsort.txt");
        for (int num : finalSorted)
            outFile << num << " ";
        outFile.close();
        cout << "Sorted result written to output_radixsort.txt\n";
    }
}
