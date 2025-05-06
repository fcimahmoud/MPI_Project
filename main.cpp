#include <mpi.h>
#include <iostream>
#include <string>

#include "quick_search.h"
#include "prime_finder.h"
#include "bitonic_sort.h"
#include "radix_sort.h"
#include "sample_sort.h"

using namespace std;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    bool running = true;
    while (running) {
        if (rank == 0) {
            cout << "\n===============================================" << endl;
            cout << "Welcome to Parallel Algorithm Simulation with MPI" << endl;
            cout << "===============================================\n" << endl;
            cout << "Please choose an algorithm to execute:" << endl;
            cout << "01 - Quick Search" << endl;
            cout << "02 - Prime Number Finding" << endl;
            cout << "03 - Bitonic Sort" << endl;
            cout << "04 - Radix Sort" << endl;
            cout << "05 - Sample Sort" << endl;
            cout << "00 - Exit" << endl;
            cout << "Enter the number of the algorithm to run: ";
        }

        int choice;
        if (rank == 0) cin >> choice;
        MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

        switch (choice) {
            case 1:
                runQuickSearch(rank, size);
                break;
            case 2:
                runPrimeFinder(rank, size);
                break;
            case 3:
                runBitonicSort(rank, size);
                break;
            case 4:
                runRadixSort(rank, size);
                break;
            case 5:
                runSampleSort(rank, size);
                break;
            case 0:
                running = false;
                break;
            default:
                if (rank == 0) cout << "Invalid option. Please choose again.\n";
        }

        if (rank == 0 && choice != 0) {
            string again;
            cout << "\nWant to try another algorithm? (Y/N): ";
            cin >> again;
            running = (again == "Y" || again == "y");
        }

        // Broadcast whether to keep running
        MPI_Bcast(&running, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
