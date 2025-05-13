#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

int findInChunk(const vector<int>& data, int target) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == target) return i;
    }
    return -1;
}

void runQuickSearch(int rank, int size) {
    string filePath;
    int target;
    vector<int> fullData;
    int totalSize = 0;

    double startTime = 0.0, endTime = 0.0;

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

        totalSize = fullData.size();
        cout << "Reading data from file... Total elements: " << totalSize << endl;
    }

    // Start timing
    MPI_Barrier(MPI_COMM_WORLD);
    startTime = MPI_Wtime();

    // Broadcast target and total size
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&totalSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute chunk sizes and displacements
    vector<int> counts(size), displs(size);
    int baseSize = totalSize / size;
    int remainder = totalSize % size;

    for (int i = 0; i < size; ++i) {
        counts[i] = baseSize + (i < remainder ? 1 : 0);
        displs[i] = (i == 0) ? 0 : displs[i - 1] + counts[i - 1];
    }

    vector<int> localData(counts[rank]);

    // Scatter data to all processes
    if (rank == 0) cout << "Distributing data using MPI_Scatterv...\n";
    MPI_Scatterv(fullData.data(), counts.data(), displs.data(), MPI_INT,
                    localData.data(), counts[rank], MPI_INT, 0, MPI_COMM_WORLD);

    // Local search
    int localIndex = findInChunk(localData, target);
    int globalIndex = std::numeric_limits<int>::max();

    if (localIndex != -1) {
        globalIndex = displs[rank] + localIndex;
        cout << "Process " << rank << " found target " << target
                << " at local index " << localIndex << ", global index " << globalIndex << endl;
    }

    // Reduce to get smallest global index
    int foundIndex;
    MPI_Allreduce(&globalIndex, &foundIndex, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    if (rank == 0) {
        double elapsed = endTime - startTime;
        cout << "Execution Time with " << size << " process(es): " << elapsed << " seconds\n";
        cout << "------------------------------\n";

        ofstream outFile("output_quicksearch.txt");
        if (foundIndex == std::numeric_limits<int>::max()) {
            cout << "Result: Value " << target << " not found in the data.\n";
            outFile << "Result: Value " << target << " not found in the data.\n";
        } else {
            cout << "Result: Value " << target << " found at index " << foundIndex << "\n";
            outFile << "Result: Value " << target << " found at index " << foundIndex << "\n";
        }
        outFile.close();
    }
}