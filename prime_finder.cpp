#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

// Check if a number is prime
bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

void runPrimeFinder(int rank, int size) {
    int low = 0, high = 0;

    double startTime = 0.0, endTime = 0.0;

    if (rank == 0) {
        cout << "------------------------------\n";
        cout << "Prime Number Finding Selected\n";
        cout << "------------------------------\n";
        cout << "Enter the lower bound of the range: ";
        cin >> low;
        cout << "Enter the upper bound of the range: ";
        cin >> high;
    }

    // Start timing after broadcasting input and before processing
    MPI_Barrier(MPI_COMM_WORLD);  // Ensure all processes are synced before timing
    startTime = MPI_Wtime();

    // Broadcast the range to all processes
    MPI_Bcast(&low, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&high, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local range for each process
    int totalNumbers = high - low + 1;
    int baseCount = totalNumbers / size;
    int remainder = totalNumbers % size;

    int localStart = low + rank * baseCount + min(rank, remainder);
    int localEnd = localStart + baseCount - 1;
    if (rank < remainder) localEnd++;

    // Each process finds primes in its subrange
    vector<int> localPrimes;
    for (int i = localStart; i <= localEnd; ++i) {
        if (isPrime(i)) {
            localPrimes.push_back(i);
        }
    }

    // Gather prime counts from all processes
    int localCount = static_cast<int>(localPrimes.size());
    cout << "[Process " << rank << "] Found " << localCount << " primes\n";
    
    vector<int> allCounts(size);
    MPI_Gather(&localCount, 1, MPI_INT, allCounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Gather prime numbers
    vector<int> displacements(size, 0);
    int totalPrimes = 0;
    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            displacements[i] = displacements[i - 1] + allCounts[i - 1];
        }
        totalPrimes = displacements[size - 1] + allCounts[size - 1];
    }

    vector<int> allPrimes(totalPrimes);
    MPI_Gatherv(localPrimes.data(), localCount, MPI_INT,
                allPrimes.data(), allCounts.data(), displacements.data(), MPI_INT,
                0, MPI_COMM_WORLD);

    // End timing after computation and reduction
    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    // Master prints the result
    if (rank == 0) {
        double elapsed = endTime - startTime;
        cout << "Execution Time with " << size << " process(es): " << elapsed << " seconds\n";
        cout << "------------------------------\n";

        cout << "Prime numbers in range [" << low << ", " << high << "]:\n";
        cout << "Total primes found: " << totalPrimes << "\n";
        cout << "Primes: ";
        for (int prime : allPrimes) {
            cout << prime << " ";
        }
        cout << endl;

        ofstream outFile("output_primefinder.txt");
        outFile << "Prime numbers in range [" << low << ", " << high << "]:\n";
        outFile << "Total primes found: " << totalPrimes << "\n";
        outFile << "Primes: ";
        for (int prime : allPrimes)
            outFile << prime << " ";
        outFile.close();
    }
}
