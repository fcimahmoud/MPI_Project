#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>

using namespace std;

// Select local samples evenly from sorted chunk
vector<int> selectSamples(const vector<int>& data, int numSamples) {
    vector<int> samples;
    int step = data.size() / (numSamples + 1);
    for (int i = 1; i <= numSamples; ++i) {
        samples.push_back(data[i * step]);
    }
    return samples;
}

void runSampleSort(int rank, int size) {
    vector<int> data;
    int totalSize = 0;

    if (rank == 0) {
        cout << "------------------------------\n";
        cout << "Sample Sort Selected\n";
        cout << "------------------------------\n";
        cout << "Please enter the path to the input file: ";
        string filePath;
        cin >> filePath;

        ifstream file(filePath);
        int num;
        while (file >> num)
            data.push_back(num);

        totalSize = data.size();
        cout << "Reading data from file... Total elements: " << totalSize << endl;
    }

    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Distribute data
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

    // Step 1: Local sort
    sort(localData.begin(), localData.end());

    // Step 2: Local sample selection
    vector<int> localSamples = selectSamples(localData, size - 1);
    vector<int> gatheredSamples((size - 1) * size);

    MPI_Gather(localSamples.data(), size - 1, MPI_INT,
                gatheredSamples.data(), size - 1, MPI_INT,
                0, MPI_COMM_WORLD);

    // Step 3: Root selects splitters
    vector<int> splitters;
    if (rank == 0) {
        sort(gatheredSamples.begin(), gatheredSamples.end());
        for (int i = 1; i < size; ++i) {
            splitters.push_back(gatheredSamples[i * (size - 1)]);
        }
    }

    // Step 4: Broadcast splitters
    splitters.resize(size - 1);
    MPI_Bcast(splitters.data(), size - 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 5: Partition local data into buckets
    vector<vector<int>> buckets(size);
    for (int val : localData) {
        int i = 0;
        while (i < splitters.size() && val > splitters[i]) i++;
        buckets[i].push_back(val);
    }

    // Flatten send buffer
    vector<int> sendCounts(size), sendBuf, sendDispls(size, 0);
    for (int i = 0; i < size; ++i) {
        sendCounts[i] = buckets[i].size();
        sendBuf.insert(sendBuf.end(), buckets[i].begin(), buckets[i].end());
    }
    for (int i = 1; i < size; ++i)
        sendDispls[i] = sendDispls[i - 1] + sendCounts[i - 1];

    // Exchange sizes
    vector<int> recvCounts(size);
    MPI_Alltoall(sendCounts.data(), 1, MPI_INT, recvCounts.data(), 1, MPI_INT, MPI_COMM_WORLD);

    // Compute recv displacements
    vector<int> recvDispls(size, 0);
    for (int i = 1; i < size; ++i)
        recvDispls[i] = recvDispls[i - 1] + recvCounts[i - 1];
    int totalRecv = recvDispls[size - 1] + recvCounts[size - 1];
    vector<int> recvBuf(totalRecv);

    // Step 6: All-to-all exchange of data
    MPI_Alltoallv(sendBuf.data(), sendCounts.data(), sendDispls.data(), MPI_INT,
                    recvBuf.data(), recvCounts.data(), recvDispls.data(), MPI_INT,
                    MPI_COMM_WORLD);

    // Step 7: Final local sort of received data
    sort(recvBuf.begin(), recvBuf.end());

    // Step 8: Gather at root
    vector<int> finalCounts(size);
    int localCount = recvBuf.size();
    MPI_Gather(&localCount, 1, MPI_INT, finalCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    vector<int> finalDispls(size, 0);
    if (rank == 0) {
        for (int i = 1; i < size; ++i)
            finalDispls[i] = finalDispls[i - 1] + finalCounts[i - 1];
    }

    vector<int> finalResult;
    if (rank == 0) finalResult.resize(totalSize);

    MPI_Gatherv(recvBuf.data(), localCount, MPI_INT,
                finalResult.data(), finalCounts.data(), finalDispls.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        cout << "Sorted Data: ";
        for (auto &&i : finalResult)
        {
            cout << i << " ";
        }cout << endl;

        ofstream out("output_samplesort.txt");
        for (int num : finalResult) out << num << " ";
        out.close();
        cout << "Result: Sorted Array is stored at file output_samplesort.txt\n";
    }
}
