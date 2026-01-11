#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>
#include "sort.h"

using std::vector;
using namespace std::chrono;

vector<int> generateRandomArray(int n) {
    static std::mt19937 gen(std::random_device{}()); // Mersenne Twister engine
    std::uniform_int_distribution<int> dist(0, 10000);

    vector<int> arr(n);
    for (auto &elem : arr) {
        elem = dist(gen);  // high-quality random integer
    }
    return arr;
}

int main() {
    srand(time(0));

    std::ofstream fout("results.csv");
    if (!fout.is_open()) {
        std::cerr << "Error: could not open results.csv\n";
        return 1;
    }

    int totalRuns = 500;

    // Write header row
    fout << "InputSize";
    for (int run = 1; run <= totalRuns; ++run) {
        fout << ",InsertionSort_Run" << run;
    }
    for (int run = 1; run <= totalRuns; ++run) {
        fout << ",QuickSort_Run" << run;
    }
    fout << "\n";

    vector<int> sizes(500);
    for (int i = 0; i < 500; ++i) sizes[i] = i + 1;

    // For each input size, run totalRuns times
    for (int n : sizes) {
        fout << n;

        // Collect insertion sort results first
        vector<long long> insertionTimes;
        insertionTimes.reserve(totalRuns);

        vector<long long> quickTimes;
        quickTimes.reserve(totalRuns);

        for (int run = 1; run <= totalRuns; ++run) {
            vector<int> arr1 = generateRandomArray(n);
            vector<int> arr2 = arr1;

            auto start = high_resolution_clock::now();
            insertionSort(arr1);
            auto end = high_resolution_clock::now();
            insertionTimes.push_back(duration_cast<microseconds>(end - start).count());

            start = high_resolution_clock::now();
            quickSort(arr2, 0, arr2.size() - 1);
            end = high_resolution_clock::now();
            quickTimes.push_back(duration_cast<microseconds>(end - start).count());
        }

        // Write insertion sort results
        for (auto t : insertionTimes) fout << "," << t;
        // Write quicksort results
        for (auto t : quickTimes) fout << "," << t;

        fout << "\n";
    }

    fout.close();
    std::cout << "Results saved to results.csv\n";
    return 0;
}
