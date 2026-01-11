# Project 1 : Basic Sorting Algorithms and Hybrid Optimization

**Course:** CS20009.04 Data Structure
**Name:** Yijia Chen
**Student Number:** 24300240127
**Date:** September 17, 2025

<br>

## Introduction

Sorting is widely used in many programs as an intermediate step. Therefore, it's necessary to analyze the efficiency of different kinds of sorting algorithms, and choose an appropriate type in application.

This project aims to:

-  Implement insertion sort, mergesort, and quicksort in C++.
-  Analyze and compare their efficiency, both in theory and by experiement.
-  Combine the advantages of these sorting algorithms to optimize the performance.

<br>

## Implementation of Sorting Algorithms

### Insertion Sort

```cpp
#include <vector>
using std::vector;

// insertion sort

template <typename T>
void insertionSort(vector<T>& a) {
    int n = a.size();
    for (int j = 1; j < n; ++j) {
        T key = a[j];              // insert a[j] into the sorted sequence a[1..j-1]
        int i = j-1;
        while (i >= 0 && a[i] > key) {
            a[i+1] = a[i];
            --i;
        }
        a[i+1] = key;
    }
}
```

Insertion sort maintains a sorted prefix `a[1..j-1]`, and insert the current element `a[j]` into it by comparing and exchanging it with its previous element repeatedly until `a[j] > a[j-1]`.

![Insertion Sort](assets/Project1/image.png)


### Mergesort

```cpp
#include <vector>
#include <algorithm>
using std::vector;

// mergesort

template <typename T>
void merge(vector<T>& a, int l, int m, int r) { // a[l..m] & a[m+1..r] are sorted
    int n1 = m-l+1, n2 = r-m;                   // lengths of 2 subarrays
    vector<T> L, R;                             // two sorted subarrays
    copy(a.begin() + l, a.begin() + m + 1, std::back_inserter(L));
    copy(a.begin() + m + 1, a.begin() + r + 1, std::back_inserter(R));

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) a[k++] = L[i++];      // copy the smaller one
        else a[k++] = R[j++];
    }

    while (i < n1) a[k++] = L[i++];             // when one of L & R is empty, copy the remainder
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
```

Mergesort follows the *divide-and-conquer* method: divide the array into two subarrays, conquer them recursively using `mergeSort`, and combine these two sorted subarrays using the auxiliary procedure `merge`.

![Mergesort](assets/Project1/image-2.png)


### Quicksort

```cpp
#include <vector>
#include <algorithm>
using std::vector;

// quicksort

template <typename T>
int partition(vector<T>& a, int l, int r) {
    T pivot = a[r];
    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (a[j] <= pivot) std::swap(a[++i], a[j]);
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
```

`partition` divides the array into two parts: all elements in one part is less than `pivot = a[r]`, and all elements in the other part is greater than `pivot = a[r]`. Then it recursively calls `quickSort` on the two subarrays.

![Quicksort](assets/Project1/image-1.png)

<br>

## Hybrid Quicksort with Insertion Sort

### Theoretical Analysis

| Algorithm       | Time $O(\cdot)$ |
|-----------------|-----------------|
| `insertionSort` | $n^2$           |
| `mergeSort`     | $n\lg n$        |
| `quickSort`     | $n\lg n$        |

In the hybrid algorithm, `quickSort` is called recursively on subarrays with more than $k$ elements. Subarrays with fewer than $k$ elements, which are left unsorted by the recursive calls of `quickSort`, are subsequently sorted by a single call to `insertionSort` directly. This takes the advantage of speediness `insertionSort` when input scale is relatively small.

<!-- If `quickSort` is called on all subarrays, whatever their length, to sort an array of length $n$, it will run in $O(n \lg n)$ expected time. Using `quickSort` to sort an array of $k$ elements takes $O(k \lg k)$ time, while using `insertionSort` takes $O(k^2)$ time. When $n$ is large with respect to $k$, there are approximately $n/k$ subarrays of length $k$. The hybrid algotithm saves $O((k^2 - k \lg k) n/k) = O(nk)$ -->

![Quicksort recursive tree](assets/Project1/image-3.png)

Consider the recursive tree of `quickSort`. 

Suppose the `partition` procedure always splits input into subarrays of sizes in a fixed 9:1 ratio, as shown in the diagram. On every level, the sum of subarray sizes $\leq n$. In particular, if none of the subproblems reaches the base case and the recursive branch terminates, then the subarrays form a partition of the original array, and their sizes add up to exactly $n$, corresponding to the top $\log_{10}n$ levels in this example.

Therefore, recursive function calls on every level takes $O(n)$ time. In the hybrid algorithm, `quickSort` only applies to $\log_k n = \lg (n/k)$ levels from the top, whose expected running time is $O(n \lg (n/k))$.

When `quickSort` halts, the array is partitiones into approximately $n/k$ unsorted subarrays, each sized $\leq k$. Let $l_i$ be the length of the $i^{th}$ subarray, where $0 \leq l_i \leq k$ and $\sum_{i} l_i = n$. Using `insertionSort` to sort these subarrays takes $$\sum_i O(l_i^2) \leq \sum_i O(k\cdot l_i) = O(\sum_i k\cdot l_i) = O(nk).$$

Combine the running time of `quickSort` and `insertionSort` above, Therefore, the hybrid sorting algorithm runs in $$O(n \lg (n/k)) + O(nk) = O(nk + n\lg(n/k))$$ expected time.

<br>

Let $f(k) = nk + n\log_2(n/k)$. Treat $n$ as a constant, and $k$ as a continuous positive variable.
$$f'(k) = n + n \cdot (-\frac{1}{k \ln 2}) = n \cdot (1 - \frac{1}{k \ln 2})$$

Set $f'(k) = 0$. Then the cirtical point $k^* = \frac{1}{\ln 2} \approx 1.4427$.
$$f''(k) = \frac{1}{k^2 \ln 2} > 0 ,\text{ for } k > 0,$$ so the critical point $k^*$ is a strict local minimum.

Because the input size $k$ of `insertionSort` must be a positive integer, it seems natural to pick $k=1$ or $k=2$ in this simplified model.

However, this result is far from accurate, mainly because in $O(nk + n\lg(n/k))$, constant factors and lower-order terms are ignored. In addition, the big-oh notation is an asymptotic upper bound, so it does not make sense when the input scale $k$ is as small as 1 or 2.


<br>

### Experimental Analysis





<br>

## Appendix : Project specification

1. Write code for insertion sort. (10 points)

2. Write code for mergesort. (10 points)

3. Write code for quicksort. (10 points)

4. The running time of quicksort can be improved in practice by taking advantage of the fast running time of insertion sort when its input is “nearly” sorted. When quicksort is called on a subarray with fewer than $k$ elements, let it simply return without sorting the subarray. After the top-level call to quicksort returns, run insertion sort on the entire array to finish the sorting process.
   Argue that this sorting algorithm runs in $O(nk + n \text{lg}(n/k))$ expected time. How should $k$ be picked, both in theory and in practice by experiments? (30 points)

5. Write code for improved version of sorting algorithm which combines quicksort with insertion sort. (20 points)

6. All document for the answers of the above questions. (20 points)
