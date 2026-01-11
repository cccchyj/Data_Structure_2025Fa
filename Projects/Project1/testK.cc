#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

using std::vector;
using namespace std::chrono;

template <typename T>
void insertionSort(vector<T>& a, int l, int r) {
    int n = r - l + 1;
    for (int j = l; j < r+1; ++j) {
        T key = a[j];
        // insert a[j] into the sorted sequence a[l..j-1]
        int i = j-1;
        while (i >= 0 && a[i] > key) {
            a[i+1] = a[i];
            --i;
        }
        a[i+1] = key;
    }
}

template <typename T>
int partition(vector<T>& a, int l, int r) {
    int pivotIndex = l + rand() % (r - l + 1);
    std::swap(a[pivotIndex], a[r]);
    
    T pivot = a[r];
    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (a[j] <= pivot) { 
        std::swap(a[++i], a[j]);
        }
    }
    std::swap(a[i+1], a[r]);
    return i+1;
}

template<typename T>
void hybridQuickSortTest(vector<T>& a, int l, int r, int k) {
    if (l < r) {
        if (r - l + 1< k) return;
        else {
            int m = partition(a, l, r);
            hybridQuickSortTest(a, l, m-1, k);
            hybridQuickSortTest(a, m+1, r, k);
        }
    }
}

template <typename T>
void hybridSortTest(vector<T>& a, int k) {
    hybridQuickSortTest(a, 0, a.size() - 1, k);
    insertionSort(a, 0, a.size() - 1);
}

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
    std::ofstream fout("resultsK.csv");
    if (!fout.is_open()) {
        std::cerr << "Error: could not open results.csv\n";
        return 1;
    }

    int totalRuns = 1000;

    // Write header row
    fout << "InputSize";
    for (int run = 1; run <= totalRuns; ++run) {
        fout << ",HybridSort_Run" << run;
    }
    fout << "\n";

    vector<int> ks(100);
    for (int i = 0; i < 100; ++i) ks[i] = i + 1;

    // For each input size, run totalRuns times
    for (int k : ks) {
        fout << k;

        // Collect insertion sort results first
        vector<long long> insertionTimes;
        insertionTimes.reserve(totalRuns);

        vector<long long> quickTimes;
        quickTimes.reserve(totalRuns);

        for (int run = 1; run <= totalRuns; ++run) {
            vector<int> arr = generateRandomArray(1000);

            auto start = high_resolution_clock::now();
            hybridSortTest(arr, k);
            auto end = high_resolution_clock::now();
            insertionTimes.push_back(duration_cast<microseconds>(end - start).count());
        }

        // Write insertion sort results
        for (auto t : insertionTimes) fout << "," << t;
        // Write quicksort results
        for (auto t : quickTimes) fout << "," << t;

        fout << "\n";
    }

    fout.close();
    std::cout << "Results saved to resultsK.csv\n";
    return 0;
}
