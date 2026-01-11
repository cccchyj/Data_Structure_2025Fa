#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <chrono>
#include "matrixMult.h"

using namespace std;
using namespace std::chrono;

using Matrix = vector<vector<double>>;


// generate random n*n matrix
Matrix randomSquareMatrix(int n) {
    static std::mt19937 gen(std::random_device{}()); // Mersenne Twister engine
    std::uniform_real_distribution<double> dist(0, 10000);
    Matrix M(n, vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            M[i][j] = dist(gen);
    return M;
}

int main() {
    ofstream fout("matrix_times.csv");

    int totalRuns = 200;

    // Write header row
    fout << "InputSize";
    for (int run = 1; run <= totalRuns; ++run) {
        fout << ",Original_Run" << run;
    }
    for (int run = 1; run <= totalRuns; ++run) {
        fout << ",Strassen_Run" << run;
    }
    fout << "\n";

    vector<int> sizes(20);
    for (int i = 0; i < 20; ++i) sizes[i] = (i + 1) * 10;

    for (int n : sizes) {
        fout << n;
        vector<long long> originalTimes;
        originalTimes.reserve(totalRuns);
        vector<long long> strassenTimes;
        strassenTimes.reserve(totalRuns);

        for (int run = 1; run <= totalRuns; ++run) {
            mat A = randomSquareMatrix(n);
            mat B = randomSquareMatrix(n);

            auto start = high_resolution_clock::now();
            matMul(A, B);
            auto end = high_resolution_clock::now();
            originalTimes.push_back(duration_cast<microseconds>(end - start).count());

            start = high_resolution_clock::now();
            matMulStrassen(A, B);
            end = high_resolution_clock::now();
            strassenTimes.push_back(duration_cast<microseconds>(end - start).count());

            if (run % 5 == 0) {
                cerr << "Run " << run << " done\n";
            }
        }
        for (auto t : originalTimes) fout << "," << t;
        for (auto t : strassenTimes) fout << "," << t;
        fout << "\n";    
        
        cerr << "======== Done n = " << n << " ========\n"; // progress display
    }

    fout.close();
    cerr << "Finished! Results written to matrix_times.csv\n";
}
