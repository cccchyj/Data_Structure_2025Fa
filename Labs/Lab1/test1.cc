#include "matrixMult.h"

int main() {
    mat A = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9,10,11,12},
        {13,14,15,16}
    };

    mat B = {
        {1, 0, 2, 0},
        {0, 1, 0, 2},
        {1, 0, 1, 0},
        {0, 1, 0, 1}
    };

    mat C = matMulStrassen(A, B);
    mat D = matMul(A, B);
    cout << "Ordinary Result:\n";
    printMat(D);
    cout << "Strassen Result:\n";
    printMat(C);
}