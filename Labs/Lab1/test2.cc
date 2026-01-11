#include "matrixMult.h"
#include <random>

// Generate a random square matrix of given size
mat randomSquareMat(int n, double minVal = -100.0, double maxVal = 100.0) {
    std::random_device rd;
    std::mt19937 gen(rd());  // random number engine
    std::uniform_real_distribution<> dist(minVal, maxVal);

    mat result(n, std::vector<double>(n));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            result[i][j] = dist(gen);
    return result;
}

int main() {
    for (int n : {2, 3, 4, 5, 8}) {
        std::cout << "Random " << n << "x" << n << " matrix:\n";
        auto A = randomSquareMat(n, -5.0, 5.0);
        auto B = randomSquareMat(n, -5.0, 5.0);
        mat C = matMulStrassen(A, B);
        mat D = matMul(A, B);
        cout << "Ordinary Result:\n";
        printMat(D);
        cout << "Strassen Result:\n";
        printMat(C);
        cout << endl;
    }
}