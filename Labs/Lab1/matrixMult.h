#pragma once
#include <iostream>
#include <vector>
#include <iomanip>
#include <stdexcept>

using std::cout; using std::endl; using std::vector;
using mat = vector<vector<double>>; // matrix

bool isSquare(const mat& m) {
    if (m.empty()) return false;
    for (const auto& row : m) {
        if (row.size() != m.size()) return false;
    }
    return true;
}

bool isValid(const mat& m) {
    if (m.empty()) return false;
    int n = m[0].size();
    for (const auto& row : m) {
        if (row.size() != n) return false; // every row.size() should be the same
    }
    return true;
}

mat createMat(int n, double val = 0.0) {
    return mat(n, vector<double>(n, val));
}

void printMat(const mat& m) {
    for (auto& row : m) {
        for (auto elem : row)
            cout << std::fixed << std::setprecision(2) << std::setw(8) << elem << ' ';
        cout << '\n';
    }
    cout << '\n';
}

bool isPowerOf2(int n) {
    return n > 0 && (n & (n - 1)) == 0;     // n = 100...00 iff isPowerOf2
}

int nextPowerOf2(int n) {
    if (n < 1) return 0;
    int res = 1;
    while (res < n) {       // until res larger than n for the first time
        res *= 2;
    }
    return res;
}

mat extendMat(const mat& m) {
    int n0 = m.size();              // original size
    int n = nextPowerOf2(n0);       // extended size
    mat res = createMat(n);
    for (int i = 0; i < n0; ++i) {
        for (int j = 0; j < n0; ++j)
            res[i][j] = m[i][j];
    }
    return res;
}

mat matAdd(const mat& a, const mat& b) {        // for ordiinary algorithm
    if (!isValid(a) || !isValid(b) || a.size() != b.size() || a[0].size() != b[0].size()){
         throw std::invalid_argument("error: matrix a & b cannot add!");
    }
    mat c = a;
    for (int i = 0; i < a.size(); ++i)
        for (int j = 0; j < a[0].size(); ++j)
            c[i][j] += b[i][j];
    return c;
}

mat matAdd(const mat& a, int ra, int ca, const mat& b, int rb, int cb, int n) {
    mat c = createMat(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            c[i][j] = a[ra+i][ca+j] + b[rb+i][cb+j];
    return c;
}

mat matSub(const mat& a, int ra, int ca, const mat& b, int rb, int cb, int n) {
    mat c = createMat(n);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            c[i][j] = a[ra+i][ca+j] - b[rb+i][cb+j];
    return c;
}

// ordinary matrix multiply

mat matMul(const mat& a, const mat& b) {
    if (!isValid(a) || !isValid(b) || a[0].size() != b.size())
        throw std::invalid_argument("Matrix a & b cannot multiply!");
    
    mat c(a.size(), vector<double>(b[0].size(), 0.0));
    for (int i = 0; i < a.size(); ++i) {
        for (int j = 0; j < b[0].size(); ++j) {
            for (int k = 0; k < b.size(); ++k)
                c[i][j] += a[i][k] * b[k][j];
        }
    }
    return c;
}


// Strassen's algorithm


mat matMulStrassenRange(const mat& a, int ra, int ca, const mat& b, int rb, int cb, int n) {
    if (n == 1) return {{a[ra][ca] * b[rb][cb]}};
    int k = n / 2;
    mat p1 = matMulStrassenRange(a, ra, ca, matSub(b, rb, cb+k, b, rb+k, cb+k, k), 0, 0, k);
    mat p2 = matMulStrassenRange(matAdd(a, ra, ca, a, ra, ca+k, k), 0, 0, b, rb+k, cb+k, k);
    mat p3 = matMulStrassenRange(matAdd(a, ra+k, ca, a, ra+k, ca+k, k), 0, 0, b, rb, cb, k);
    mat p4 = matMulStrassenRange(a, ra+k, ca+k, matSub(b, rb+k, cb, b, rb, cb, k), 0, 0, k);
    mat p5 = matMulStrassenRange(matAdd(a, ra, ca, a, ra+k, ca+k, k), 0, 0, 
                                 matAdd(b, rb, cb, b, rb+k, cb+k, k), 0, 0, k);
    mat p6 = matMulStrassenRange(matSub(a, ra, ca+k, a, ra+k, ca+k, k), 0, 0, 
                                 matAdd(b, rb+k, cb, b, rb+k, cb+k, k), 0, 0, k);
    mat p7 = matMulStrassenRange(matSub(a, ra, ca, a, ra+k, ca, k), 0, 0, 
                                 matAdd(b, rb, cb, b, rb, cb+k, k), 0, 0, k);
    mat c = createMat(n);
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            c[i][j] = p5[i][j] + p4[i][j] - p2[i][j] + p6[i][j];
            c[i][j+k] = p1[i][j] + p2[i][j];
            c[i+k][j] = p3[i][j] + p4[i][j];
            c[i+k][j+k] = p5[i][j] + p1[i][j] - p3[i][j] - p7[i][j];
        }
    }
    return c;
}

mat matMulStrassen(const mat& a, const mat& b) {
    if (!isValid(a) || !isValid(b))
        throw std::invalid_argument("error: matrices not valid");
    if (!isSquare(a) || !isSquare(b))
        throw std::invalid_argument("error: matrices not square");
    if (a.size() != b.size())
        throw std::invalid_argument("error: a.size() != b.size()");
    int n = a.size();
    if (isPowerOf2(n)) {
        return matMulStrassenRange(a, 0, 0, b, 0, 0, n);
    } else {
        return matMulStrassenRange(extendMat(a), 0, 0, extendMat(b), 0, 0, nextPowerOf2(n));
    }
}