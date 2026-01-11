#pragma once
#include <vector>
#include <algorithm>

using std::vector;

// insertion sort

template <typename T>
void insertionSort(vector<T>& a) {
    int n = a.size();
    for (int j = 1; j < n; ++j) {
        T key = a[j];
        // insert a[j] into the sorted sequence a[1..j-1]
        int i = j-1;
        while (i >= 0 && a[i] > key) {
            a[i+1] = a[i];
            --i;
        }
        a[i+1] = key;
    }
}

// merge sort

template <typename T>
void merge(vector<T>& a, int l, int m, int r) {
    // a[l..m] & a[m+1..r] are sorted
    int n1 = m-l+1, n2 = r-m; // lengths of 2 subarrays
    vector<T> L, R; // two sorted subarrays
    copy(a.begin() + l, a.begin() + m + 1, std::back_inserter(L));
    copy(a.begin() + m + 1, a.begin() + r + 1, std::back_inserter(R));

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) a[k++] = L[i++]; // copy the smaller one
        else a[k++] = R[j++];
    }
    // when one of L and R is empty, copy the remaining elements
    while (i < n1) a[k++] = L[i++];
    while (j < n2) a[k++] = R[j++];
}

template <typename T>
void mergeSort(vector<T>& a, int l, int r) {
    if (l < r) {
        int m = (l + r) / 2;
        mergeSort(a, l, m);
        mergeSort(a, m+1, r);
        merge(a, l, m, r);
    }
}

// randomized quick sort

template <typename T>
int partition(vector<T>& a, int l, int r) {
    int pivotIndex = rand() % (r - l + 1);
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

template <typename T>
void quickSort(vector<T>& a, int l, int r) {
    if (l < r) {
        int m = partition(a, l, r);
        quickSort(a, l, m-1);
        quickSort(a, m+1, r);
    }
}