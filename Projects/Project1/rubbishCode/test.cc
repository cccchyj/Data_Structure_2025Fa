#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include "sort.h"

using std::cin; using std::cout; using std::endl; 
using std::vector; using std::string;
using namespace std::chrono;

vector<int> generateRandomArray(int n) {
    vector<int> arr(n);
    for (auto &elem : arr) elem = rand(); // the & here is important!!
    return arr;
}

int main() {
    srand(time(0));

    std::ofstream fout("results.csv"); // open CSV file for writing
    if (!fout.is_open()) {
        std::cerr << "Error: could not open results.csv\n";
        return 1;
    }

    // Write header row
    fout << "InputSize,InsertionSort(ms),QuickSort(ms)\n";

    vector<int> sizes(100);
    for (int i = 0; i < 100; ++i) sizes[i] = i + 1;

    for (int n : sizes) {
        vector<int> arr1 = generateRandomArray(n);
        vector<int> arr2 = arr1;

        auto start = high_resolution_clock::now();
        insertionSort(arr1);
        auto end = high_resolution_clock::now();
        auto durationInsertion = duration_cast<microseconds>(end - start).count();

        start = high_resolution_clock::now();
        quickSort(arr2, 0, arr2.size() - 1);
        end = high_resolution_clock::now();
        auto durationQuick = duration_cast<microseconds>(end - start).count();

        fout << n << "," << durationInsertion << "," << durationQuick << "\n";
    }

    fout.close();
    std::cout << "Results saved to results.csv\n";
}