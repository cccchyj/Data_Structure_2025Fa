#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <fstream>
#include <cstdlib>
#include <random>

// Create a global random engine
std::mt19937 rng(std::random_device{}());  
std::uniform_int_distribution<int> dist(0, 1000000);

using namespace std;
using namespace std::chrono;

// ================= Sorting Algorithms =================
template <typename T>
void insertionSort(vector<T>& a) {
    int n = a.size();
    for (int j = 1; j < n; ++j) {
        T key = a[j];
        int i = j - 1;
        while (i >= 0 && a[i] > key) {
            a[i+1] = a[i];
            --i;
        }
        a[i+1] = key;
    }
}

template <typename T>
int randomizedPartition(vector<T>& a, int l, int r) {
    int pivotIndex = l + rand() % (r - l + 1);
    swap(a[pivotIndex], a[r]);
    T pivot = a[r];
    int i = l - 1;
    for (int j = l; j < r; ++j) {
        if (a[j] <= pivot) swap(a[++i], a[j]);
    }
    swap(a[i+1], a[r]);
    return i + 1;
}

template <typename T>
void quickSort(vector<T>& a, int l, int r) {
    if (l < r) {
        int m = randomizedPartition(a, l, r);
        quickSort(a, l, m-1);
        quickSort(a, m+1, r);
    }
}

// ================= Helpers =================
double mean(const vector<double>& v) {
    return accumulate(v.begin(), v.end(), 0.0) / v.size();
}

double median(vector<double> v) {
    sort(v.begin(), v.end());
    size_t n = v.size();
    if (n % 2 == 0) return 0.5 * (v[n/2 - 1] + v[n/2]);
    else return v[n/2];
}

vector<double> removeOutliersIQR(vector<double> v) {
    if (v.size() < 4) return v;
    sort(v.begin(), v.end());
    size_t n = v.size();
    double q1 = v[n/4];
    double q3 = v[3*n/4];
    double iqr = q3 - q1;
    double low = q1 - 1.5 * iqr;
    double high = q3 + 1.5 * iqr;
    vector<double> clean;
    for (double x : v) if (x >= low && x <= high) clean.push_back(x);
    return clean.empty() ? v : clean;
}

double fitSlope(const vector<double>& x, const vector<double>& y) {
    double num = 0, den = 0;
    for (size_t i = 0; i < x.size(); ++i) {
        num += y[i] * x[i];
        den += x[i] * x[i];
    }
    return num / den;
}

double SSE(const vector<double>& y, const vector<double>& pred) {
    double sum = 0;
    for (size_t i = 0; i < y.size(); ++i)
        sum += (y[i] - pred[i]) * (y[i] - pred[i]);
    return sum;
}

// ================= Random Input Generator =================
vector<int> generateRandomArray(int n) {
    vector<int> arr(n);
    for (int &x : arr) {
        x = dist(rng);  // generate random number
    }
    return arr;
}

// ================= Main =================
int main() {
    srand(time(0));

    int runs = 50;     // number of runs per input size
    int maxN = 200;    // max input size
    vector<int> ns;
    for (int i = 1; i <= maxN; ++i) ns.push_back(i);

    vector<double> ins_vals, quick_vals;

    ofstream fout("results.csv");
    fout << "n,InsertionMedian,QuickMedian,InsertionFit,QuickFit,PiecewiseFit\n";

    for (int n : ns) {
        vector<double> timesIns, timesQuick;

        for (int r = 0; r < runs; ++r) {
            vector<int> arr = generateRandomArray(n);
            vector<int> arr2 = arr;

            auto start = high_resolution_clock::now();
            insertionSort(arr);
            auto end = high_resolution_clock::now();
            timesIns.push_back(duration_cast<microseconds>(end - start).count());

            start = high_resolution_clock::now();
            quickSort(arr2, 0, arr2.size()-1);
            end = high_resolution_clock::now();
            timesQuick.push_back(duration_cast<microseconds>(end - start).count());
        }

        timesIns = removeOutliersIQR(timesIns);
        timesQuick = removeOutliersIQR(timesQuick);

        double medIns = median(timesIns);
        double medQuick = median(timesQuick);

        ins_vals.push_back(medIns);
        quick_vals.push_back(medQuick);
    }

    // Fit models globally
    vector<double> n2, nlogn;
    for (double n : ns) {
        n2.push_back(n * n);
        nlogn.push_back(n * log2(max(2.0, n))); // avoid log(0)
    }
    double c1 = fitSlope(n2, ins_vals);
    double c2 = fitSlope(nlogn, quick_vals);

    // Build predictions
    vector<double> ins_fit, quick_fit;
    for (size_t i = 0; i < ns.size(); ++i) {
        ins_fit.push_back(c1 * n2[i]);
        quick_fit.push_back(c2 * nlogn[i]);
    }

    // Find best cut n0 for piecewise model
    double bestSSE = 1e18;
    int bestCut = -1;
    vector<double> bestPiece;
    for (size_t cut = 5; cut < ns.size()-5; ++cut) {
        vector<double> leftX(ns.begin(), ns.begin()+cut+1);
        vector<double> rightX(ns.begin()+cut+1, ns.end());
        vector<double> leftY(ins_vals.begin(), ins_vals.begin()+cut+1);
        vector<double> rightY(ins_vals.begin()+cut+1, ins_vals.end());

        vector<double> leftX2, rightXlog;
        for (double n : leftX) leftX2.push_back(n*n);
        for (double n : rightX) rightXlog.push_back(n*log2(n));

        double cL = fitSlope(leftX2, leftY);
        double cR = fitSlope(rightXlog, rightY);

        vector<double> preds(ns.size());
        for (size_t i = 0; i <= cut; ++i) preds[i] = cL * ns[i] * ns[i];
        for (size_t i = cut+1; i < ns.size(); ++i) preds[i] = cR * ns[i] * log2(ns[i]);

        double err = SSE(ins_vals, preds);
        if (err < bestSSE) {
            bestSSE = err;
            bestCut = ns[cut];
            bestPiece = preds;
        }
    }

    // Save everything
    for (size_t i = 0; i < ns.size(); ++i) {
        fout << ns[i] << ","
             << ins_vals[i] << ","
             << quick_vals[i] << ","
             << ins_fit[i] << ","
             << quick_fit[i] << ","
             << bestPiece[i] << "\n";
    }

    fout.close();

    cout << "Results saved to results.csv\n";
    cout << "Insertion fit: T(n) ≈ " << c1 << " * n^2\n";
    cout << "Quick fit: T(n) ≈ " << c2 << " * n log n\n";
    cout << "Best cut n0 ≈ " << bestCut << endl;
}
