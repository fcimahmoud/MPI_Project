# Parallel Search and Sorting Algorithms with MPI

This project demonstrates the implementation of parallel search and sorting algorithms using **MPI (Message Passing Interface)** in C++. It showcases efficient distributed algorithm design and performance analysis through practical, hands-on simulations.

## 📌 Features

* Modular CLI to select and run any algorithm
* MPI-based parallel implementations:

  * Quick Search
  * Prime Number Finding
  * Bitonic Sort
  * Radix Sort
  * Sample Sort (Bonus)
* Dynamic input loading via text files
* Output sorted results to file (for sorting algorithms)
* Performance measured using `MPI_Wtime()`

## 🧰 Prerequisites

* **Microsoft MPI** (or any MPI implementation)
* **g++** compiler (MinGW or equivalent for Windows)

## 🚀 Compilation and Execution (Windows)

### Step 1: Compile

```bash
build.bat
```

> This compiles using Microsoft MPI include and lib directories.

### Step 2: Run with 4 Processes

```bash
mpiexec -n 4 a.exe
```

## 🧪 Input Format

Each algorithm reads from a plain `.txt` file:

* **Quick Search / Sorting:** One number per line
* **Prime Finder:** Enter start and end range via CLI

Example for input file:

```
90 12 33 5 87 56 41 21 69 50
```

## 📋 Usage Demo

```bash
===============================================
Welcome to Parallel Algorithm Simulation with MPI
===============================================
Please choose an algorithm to execute:
01 - Quick Search
02 - Prime Number Finding
03 - Bitonic Sort
04 - Radix Sort
05 - Sample Sort
00 - Exit
Enter the number of the algorithm to run: 
```

## 📂 Project Structure

```
- main.cpp
- quick_search.cpp
- prime_finder.cpp
- bitonic_sort.cpp
- radix_sort.cpp
- sample_sort.cpp
- build.bat
- input_*.txt
- output_*.txt
```

## 📈 Performance

Performance analysis is included in the documentation:

* Speedup and efficiency with varying process counts and data sizes
* Comparison of communication overheads

## 📎 License

This project is for educational and academic use.

---