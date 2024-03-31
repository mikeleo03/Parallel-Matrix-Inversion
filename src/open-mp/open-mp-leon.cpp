#include <iostream>
#include <omp.h>

using namespace std;

// Struct to represent the Gaussian Matrix
struct GaussianMatrix {
    int size;
    double *mat;
};

// Initialize the matrix based on user input
void initialize_matrix(GaussianMatrix& matrix) {
    // Get the matrix size
    cin >> matrix.size;

    // Allocate memory for the matrix
    matrix.mat = new double[matrix.size * matrix.size * 2];
}

// Function to get the input matrix from user input and init identity
void input_matrix(GaussianMatrix& matrix) {
    // Get the user input
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = 0; j < matrix.size; ++j) {
            cin >> matrix.mat[i * matrix.size * 2 + j];
        }

        // Initialize right-hand side to identity matrix
        for (int j = matrix.size; j < matrix.size * 2; ++j) {
            matrix.mat[i * matrix.size * 2 + j] = (j == i + matrix.size) ? 1 : 0;
        }
    }
}

// Free the memory allocated for the matrix
void deallocate_matrix(GaussianMatrix& matrix) {
    delete[] matrix.mat;
}

// Perform partial pivoting and reduce the matrix to a diagonal form
void parallel_partial_pivot(GaussianMatrix& matrix) {
    double *mat = matrix.mat;

    // Perform Gaussian elimination with pivoting
    for (int i = 0; i < matrix.size; ++i) {
        int pivot_row = i;
        double pivot_value = abs(mat[i * matrix.size * 2 + i]);
        
        // Find the maximum element in the current column
        for (int row = i + 1; row < matrix.size; ++row) {
            if (abs(mat[row * matrix.size * 2 + i]) > pivot_value) {
                pivot_row = row;
                pivot_value = abs(mat[row * matrix.size * 2 + i]);
            }
        }

        // Swap rows if necessary
        for (int k = 0; k < matrix.size * 2; ++k) {
            swap(mat[i * matrix.size * 2 + k], mat[pivot_row * matrix.size * 2 + k]);
        }

        // Parallelize row operations to create upper triangular matrix
        #pragma omp parallel for shared(mat) // Parallelize row operations
        for (int row = i + 1; row < matrix.size; ++row) {
            double factor = mat[row * matrix.size * 2 + i] / mat[i * matrix.size * 2 + i];
            for (int j = i; j < matrix.size * 2; ++j) {
                mat[row * matrix.size * 2 + j] -= mat[i * matrix.size * 2 + j] * factor;
            }
        }
    }
}

// Reduce the matrix to a unit matrix
void parallel_reduce_to_unit(GaussianMatrix& matrix) {
    double *mat = matrix.mat;

    // Back substitution to obtain the inverse matrix
    for (int i = matrix.size - 1; i >= 0; --i) {
        double factor = mat[i * matrix.size * 2 + i];
        
        #pragma omp parallel for shared(mat, factor)
        for (int j = 0; j < matrix.size * 2; ++j) {
            mat[i * matrix.size * 2 + j] /= factor;
        }

        // Parallelize back substitution
        #pragma omp parallel for shared(mat)
        for (int row = i - 1; row >= 0; --row) {
            double factor = mat[row * matrix.size * 2 + i];
            for (int j = matrix.size * 2 - 1; j >= i; --j) {
                mat[row * matrix.size * 2 + j] -= mat[i * matrix.size * 2 + j] * factor;
            }
        }
    }
}

// Function to print the resulting matrix
void print_result(GaussianMatrix& matrix) {
    cout << matrix.size << endl;
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = matrix.size; j < 2 * matrix.size; ++j) {
            cout << matrix.mat[i * matrix.size * 2 + j] << " ";
        }
        cout << endl;
    }
}

int main() {
    // Initaite object
    GaussianMatrix matrix;
    /* int num_threads = 16; // Set the desired number of threads
    omp_set_num_threads(num_threads); */
    
    // Initialize the matrix based on user input
    initialize_matrix(matrix);
    
    // Get the input matrix from user input
    input_matrix(matrix);
    
    // Perform partial pivoting
    parallel_partial_pivot(matrix);
    
    // Reduce the matrix to a unit matrix
    parallel_reduce_to_unit(matrix);
    
    // Print the result
    print_result(matrix);
    
    // Free the memory allocated for the matrix
    deallocate_matrix(matrix);
    
    return 0;
}
