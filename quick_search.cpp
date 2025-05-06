#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

int findInChunk(const vector<int>& data, int target) {
    // for (auto &&i : data)
    // {
    //     cout << i << " ";
    // }cout << endl;
    
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == target) return i;
    }
    return -1;
}

void runQuickSearch(int rank, int size) {
    string filePath;
    int target;
    vector<int> fullData;

    // Master reads input
    if (rank == 0) {
        cout << "------------------------------\n";
        cout << "Quick Search Selected\n";
        cout << "------------------------------\n";
        cout << "Please enter the path to the input file: ";
        cin >> filePath;
        cout << "Enter Search Target: ";
        cin >> target;

        ifstream file(filePath);
        if (!file) {
            cerr << "Error opening file.\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        int num;
        while (file >> num) {
            fullData.push_back(num);
        }

        cout << "Reading data from file... Total elements: " << fullData.size() << endl;
        cout << "Reading data completed successfully: ";
        for (auto &&i : fullData)
        {
            cout << i << " ";
        }
        cout << endl;
    }

    // Broadcast target value and total size
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int totalSize = static_cast<int>(fullData.size());
    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute chunk sizes
    int baseSize = totalSize / size;
    int remainder = totalSize % size;
    int localSize = (rank < remainder) ? baseSize + 1 : baseSize;

    vector<int> localData(localSize);

    if (rank == 0) {
        // Master sends chunks manually
        cout << "Distributing data across processes..." << endl;
        int offset = 0;
        for (int i = 0; i < size; ++i) {
            int sendCount = (i < remainder) ? baseSize + 1 : baseSize;
            if (i == 0) {
                copy(fullData.begin(), fullData.begin() + sendCount, localData.begin());
            } else {
                MPI_Send(fullData.data() + offset, sendCount, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            offset += sendCount;
        }
    } else {
        MPI_Recv(localData.data(), localSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Each process searches its chunk
    if (rank == 0) cout << "Each process is searching its assigned range..." << endl;
    int localIndex = findInChunk(localData, target);
    int globalIndex = INT_MAX; // Initialize to a large value

    // Compute global index if found
    if (localIndex != -1) {
        int offset = 0;
        for (int i = 0; i < rank; ++i) {
            offset += (i < remainder) ? baseSize + 1 : baseSize;
        }
        globalIndex = offset + localIndex;
        cout << "Process " << rank << " found target " << target 
                << " at local index " << localIndex << ", global index " << globalIndex << endl;
    }

    // Reduce to find the smallest valid index
    int foundIndex;
    MPI_Allreduce(&globalIndex, &foundIndex, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    // Master prints result
    if (rank == 0) {
        ofstream outFile("output_quicksearch.txt");
        if (foundIndex == -1) {
            cout << "Result: Value " << target << " not found in the data." << endl;
            outFile << "Result: Value " << target << " not found in the data." << endl;
        } else {
            cout << "Result: Value " << target << " found at index " << foundIndex << endl;
            outFile << "Result: Value " << target << " found at index " << foundIndex << endl;
        }
        outFile.close();
    }
}
