#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

// ---------- Insertion Sort ----------
void insertionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ---------- Quicksort ----------
int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int p = partition(arr, low, high);
        quickSort(arr, low, p - 1);
        quickSort(arr, p + 1, high);
    }
}

// ---------- Helper: Generate random data ----------
vector<int> generateRandomArray(int n) {
    vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = rand() % (10 * n);
    return arr;
}

// ---------- Main Experiment ----------
int main() {
    srand(time(0));

    vector<int> sizes = {10, 50, 100, 200, 500, 1000, 2000, 5000};
    cout << "InputSize,InsertionSort(ms),QuickSort(ms)" << endl;

    for (int n : sizes) {
        vector<int> arr1 = generateRandomArray(n);
        vector<int> arr2 = arr1;

        // Time Insertion Sort
        auto start = high_resolution_clock::now();
        insertionSort(arr1);
        auto end = high_resolution_clock::now();
        auto durationInsertion = duration_cast<microseconds>(end - start).count();

        // Time Quicksort
        start = high_resolution_clock::now();
        quickSort(arr2, 0, arr2.size() - 1);
        end = high_resolution_clock::now();
        auto durationQuick = duration_cast<microseconds>(end - start).count();

        cout << n << "," << durationInsertion << "," << durationQuick << endl;
    }

    return 0;
}
