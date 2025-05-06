# Parallel Search and Sorting Algorithms with MPI

This project demonstrates the implementation of parallel search and sorting algorithms using **MPI (Message Passing Interface)** in C++.

## 📌 Features

* Modular CLI to select and run any algorithm
* MPI-based parallel implementations:

  * Quick Search
  * Prime Number Finding
  * Bitonic Sort
  * Radix Sort
  * Sample Sort
* Dynamic input loading via text files
* Output sorted results to file (for sorting algorithms)

## 🧰 Prerequisites

* **Microsoft MPI**
* **g++** compiler (MinGW)

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

* **Quick Search / Sorting:** All numbers on one line seperated by space
* **Prime Finder:** Enter start and end range via CLI

Example for input file:

```
90 12 33 5 87 56 41 21 69 50
```

## 📋 Usage Demo
### 🧭 Main Menu
```mathematica
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
### 🔍 01 - Quick Search
```sql
------------------------------
Quick Search Selected
------------------------------
Please enter the path to the input file: input_quicksearch.txt
Enter Search Target: 50
Reading data from file... Total elements: 12
Reading data completed successfully: 10 20 30 40 50 60 70 80 90 100 55 25
Distributing data across processes...
Each process is searching its assigned range...
Process 1 found target 50 at local index 1, global index 4
Result: Value 50 found at index 4
```

Or if the target is not found:
```kotlin
Result: Value 50 not found in the data.
```
### 🔢 02 - Prime Number Finding
```sql
------------------------------
Prime Number Finding Selected
------------------------------
Enter start of range: 1
Enter end of range: 100
Distributing number ranges to processes...
[Process 1] Found 6 primes
[Process 2] Found 6 primes
[Process 3] Found 4 primes
[Process 0] Found 9 primes
Total primes found: 25
Primes: 2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97
```
### 📈 03 - Bitonic Sort
```sql
------------------------------
Bitonic Sort Selected
------------------------------
Please enter the path to the input file: input_bitonicsort.txt
Reading data from file... Total elements: 8
Sorted Data: 5 12 21 33 41 56 87 90
Result: Sorted Array is stored at file output_bitonicsort.txt
```

Output file content (example):
```bash
1 3 5 12 22 34 56 99
```

### ⚙️ 04 - Radix Sort
```sql
------------------------------
Radix Sort Selected
------------------------------
Please enter the path to the input file: input_radixsort.txt
Reading data from file... Total elements: 10
Sorted Data: 5 12 21 33 41 50 56 69 87 90
Result: Sorted Array is stored at file output_radixsort.txt
```

Output file content:
```bash
5 12 21 33 41 50 56 69 87 90
```

### 🧪 05 - Sample Sort
```sql
------------------------------
Sample Sort Selected
------------------------------
Please enter the path to the input file: input_samplesort.txt
Reading data from file... Total elements: 10
Sorted Data: 5 12 21 33 41 50 56 69 87 90
Result: Sorted Array is stored at file output_samplesort.txt
```
Output file content:
```bash
5 12 21 33 41 50 56 69 87 90
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

## 📎 License

This project is for educational and academic use.

---