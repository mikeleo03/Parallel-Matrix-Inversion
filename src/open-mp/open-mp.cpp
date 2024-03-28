#include <iostream>
#include <omp.h>

using namespace std;

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
    int i, j;
    double d;
    int n = matrix.size;

    // Perform pivoting
    for (i = 0; i < n - 1; ++i) {
        int pivot_row = i;
        double pivot_value = matrix.mat[i * matrix.size * 2 + i];

        // Find the row with the maximum value in the current column
        for (int row = i + 1; row < n; ++row) {
            if (abs(matrix.mat[row * matrix.size * 2 + i]) > abs(pivot_value)) {
                pivot_row = row;
                pivot_value = matrix.mat[row * matrix.size * 2 + i];
            }
        }

        // Swap rows if necessary
        if (pivot_row != i) {
            for (j = 0; j < 2 * n; ++j) {
                swap(matrix.mat[i * matrix.size * 2 + j], matrix.mat[pivot_row * matrix.size * 2 + j]);
            }
        }

        // Eliminate the elements below the pivot
        for (int row = i + 1; row < n; ++row) {
            d = matrix.mat[row * matrix.size * 2 + i] / matrix.mat[i * matrix.size * 2 + i];
            for (j = i; j < 2 * n; ++j) {
                matrix.mat[row * matrix.size * 2 + j] -= matrix.mat[i * matrix.size * 2 + j] * d;
            }
        }
    }
}

// Reduce the matrix to a unit matrix
void parallel_reduce_to_unit(GaussianMatrix& matrix) {
    int i, j;
    double d;
    int n = matrix.size;

    // Perform back substitution
    for (i = n - 1; i >= 0; --i) {
        d = matrix.mat[i * matrix.size * 2 + i];
        for (j = i; j < 2 * n; ++j) {
            matrix.mat[i * matrix.size * 2 + j] /= d;
        }

        // Eliminate the elements above the pivot
        for (int row = 0; row < i; ++row) {
            d = matrix.mat[row * matrix.size * 2 + i];
            for (j = i; j < 2 * n; ++j) {
                matrix.mat[row * matrix.size * 2 + j] -= matrix.mat[i * matrix.size * 2 + j] * d;
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

int main(int argc, char** argv) {
    // Initaite object
    GaussianMatrix matrix;

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
