#pragma once
#include <vector>
#include <algorithm>

using std::vector;

#define k 36

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
void hybridQuickSort(vector<T>& a, int l, int r) {
    if (l < r) {
        if (r - l + 1< k) return;
        else {
            int m = partition(a, l, r);
            hybridQuickSort(a, l, m-1);
            hybridQuickSort(a, m+1, r);
        }
    }
}

template <typename T>
void hybridSort(vector<T>& a) {
    hybridQuickSort(a, 0, a.size() - 1, k);
    insertionSort(a, 0, a.size() - 1);
}