#include <omp.h>
#include <iostream>
#include <algorithm> // Untuk std::swap

using namespace std;

// Gaussian Matrix Struct
struct GaussianMatrix {
    int size;
    double *mat;
};

//Inisialisasi matrix
void initialize_matrix(GaussianMatrix& matrix) {
    cin >> matrix.size;
    matrix.mat = new double[matrix.size * matrix.size * 2];
}

//Minta input matrix
void input_matrix(GaussianMatrix& matrix) {
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = 0; j < matrix.size; ++j) {
            cin >> matrix.mat[i * matrix.size * 2 + j];
        }
        for (int j = matrix.size; j < matrix.size * 2; ++j) {
            matrix.mat[i * matrix.size * 2 + j] = (j == i + matrix.size) ? 1 : 0;
        }
    }
}

//Ngebagi pivot 
void divideRowPivot(GaussianMatrix& matrix, int row) {
    double pivot = matrix.mat[row * matrix.size * 2 + row];
    #pragma omp parallel for
    for(int idx = 0; idx < matrix.size * 2; idx++) {
        matrix.mat[row * matrix.size * 2 + idx] /= pivot;
    }
}

//Buat ngelakuin reduksi baris bagian atas
void reduksiBarisAtas(GaussianMatrix& matrix, int row) {
    #pragma omp parallel for
    for(int idx = row + 1; idx < matrix.size; idx++) {
        double factor = matrix.mat[idx * matrix.size * 2 + row];
        for(int idxColom = 0; idxColom < matrix.size * 2; idxColom++) {
            matrix.mat[idx * matrix.size * 2 + idxColom] -= factor * matrix.mat[row * matrix.size * 2 + idxColom];
        }
    }
}

//Buat ngelakuin reduksi baris bagian bawah
void reduksiBarisBawah(GaussianMatrix& matrix, int row) {
    #pragma omp parallel for
    for(int idx = row - 1; idx >= 0; idx--) {
        double factor = matrix.mat[idx * matrix.size * 2 + row];
        for(int idxColom = 0; idxColom < matrix.size * 2; idxColom++) {
            matrix.mat[idx * matrix.size * 2 + idxColom] -= factor * matrix.mat[row * matrix.size * 2 + idxColom];
        }
    }
}

//Ngelakuin swap kalo misal yang mau dibagi tuh 0
void swapPivot(GaussianMatrix& matrix, int row) {
    for(int idxrow = row + 1; idxrow < matrix.size; idxrow++) {
        if(matrix.mat[idxrow * matrix.size * 2 + row] != 0) {
            for(int col = 0; col < matrix.size * 2; col++) {
                swap(matrix.mat[row * matrix.size * 2 + col], matrix.mat[idxrow * matrix.size * 2 + col]);
            }
            break;
        }
    }
}

//Main utama
void mainInverseParallelGaussJordan(GaussianMatrix& matrix, int num_threads) {
    omp_set_num_threads(num_threads);
    for(int i = 0; i < matrix.size; i++) {
        swapPivot(matrix, i);
        divideRowPivot(matrix, i);
        reduksiBarisAtas(matrix, i);
    }
    for (int i = matrix.size - 1; i >= 0; i--) {
        reduksiBarisBawah(matrix, i);
    }
}

//Buat output matrix
void print_result(GaussianMatrix& matrix) {
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = matrix.size; j < 2 * matrix.size; ++j) {
            cout << matrix.mat[i * matrix.size * 2 + j] << " ";
        }
        cout << endl;
    }
}

int main() {
    int num_threads=4; 
    GaussianMatrix matrix;
    initialize_matrix(matrix);
    input_matrix(matrix);
    mainInverseParallelGaussJordan(matrix, num_threads);
    print_result(matrix);
    delete[] matrix.mat; 
    return 0;
}