#include <iostream>
#include <mpi.h>

using namespace std;

struct GaussianMatrix {
    int size;
    double *mat;
};

// Initialize the matrix based on user input
void initialize_matrix(GaussianMatrix& matrix, int my_rank, int num_procs) {
    if (my_rank == 0) {
        cin >> matrix.size;
    }
    
    // Broadcast the size of the matrix to all processes
    MPI_Bcast(&matrix.size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the matrix
    matrix.mat = new double[matrix.size * matrix.size * 2];
}

// Function to get the input matrix from user input and init identity
void input_matrix(GaussianMatrix& matrix, int my_rank, int num_procs) {
    if (my_rank == 0) {
        // Get the user input
        for (int i = 0; i < matrix.size; ++i) {
            for (int j = 0; j < matrix.size; ++j) {
                cin >> matrix.mat[i * matrix.size * 2 + j];
            }
            // Initialize right-hand side to identity matrix
            matrix.mat[i * matrix.size * 2 + i + matrix.size] = 1;
        }
    }

    // Broadcast the input matrix to all processes
    MPI_Bcast(matrix.mat, matrix.size * matrix.size * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

// Free the memory allocated for the matrix
void deallocate_matrix(GaussianMatrix& matrix) {
    delete[] matrix.mat;
}

// Perform partial pivoting and reduce the matrix to a diagonal form
void parallel_partial_pivot(GaussianMatrix& matrix, int my_rank, int num_procs) {
    int i, j;
    double d;
    int n = matrix.size;

    // Perform pivoting locally within each process
    for (i = 0; i < n - 1; ++i) {
        int pivot_row = i;
        double pivot_value = matrix.mat[i * n * 2 + i];

        // Find the row with the maximum value in the current column
        for (int row = i + 1; row < n; ++row) {
            if (abs(matrix.mat[row * n * 2 + i]) > abs(pivot_value)) {
                pivot_row = row;
                pivot_value = matrix.mat[row * n * 2 + i];
            }
        }

        // Swap rows if necessary
        if (pivot_row != i) {
            for (j = 0; j < 2 * n; ++j) {
                swap(matrix.mat[i * n * 2 + j], matrix.mat[pivot_row * n * 2 + j]);
            }
        }

        // Eliminate the elements below the pivot
        for (int row = i + 1; row < n; ++row) {
            d = matrix.mat[row * n * 2 + i] / matrix.mat[i * n * 2 + i];
            for (j = i; j < 2 * n; ++j) {
                matrix.mat[row * n * 2 + j] -= matrix.mat[i * n * 2 + j] * d;
            }
        }
    }

    // Synchronize all processes before proceeding to the next step
    MPI_Barrier(MPI_COMM_WORLD);
}

// Reduce the matrix to a unit matrix
void parallel_reduce_to_unit(GaussianMatrix& matrix, int my_rank, int num_procs) {
    int i, j;
    double d;
    int n = matrix.size;

    // Perform back substitution locally within each process
    for (i = n - 1; i >= 0; --i) {
        d = matrix.mat[i * n * 2 + i];
        for (j = i; j < 2 * n; ++j) {
            matrix.mat[i * n * 2 + j] /= d;
        }

        // Eliminate the elements above the pivot
        for (int row = 0; row < i; ++row) {
            d = matrix.mat[row * n * 2 + i];
            for (j = i; j < 2 * n; ++j) {
                matrix.mat[row * n * 2 + j] -= matrix.mat[i * n * 2 + j] * d;
            }
        }
    }

    // Synchronize all processes before proceeding
    MPI_Barrier(MPI_COMM_WORLD);
}

// Function to print the resulting matrix
void print_result(GaussianMatrix& matrix, int my_rank) {
    if (my_rank == 0) {
        cout << matrix.size << endl;
        for (int i = 0; i < matrix.size; ++i) {
            for (int j = matrix.size; j < 2 * matrix.size; ++j) {
                cout << matrix.mat[i * matrix.size * 2 + j] << " ";
            }
            cout << endl;
        }
    }
}

int main(int argc, char** argv) {
    int my_rank, num_procs;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    
    GaussianMatrix matrix;
    
    // Initialize the matrix based on user input
    initialize_matrix(matrix, my_rank, num_procs);
    
    // Get the input matrix from user input
    input_matrix(matrix, my_rank, num_procs);
    
    // Perform partial pivoting
    parallel_partial_pivot(matrix, my_rank, num_procs);
    
    // Reduce the matrix to a unit matrix
    parallel_reduce_to_unit(matrix, my_rank, num_procs);
    
    // Print the result
    print_result(matrix, my_rank);
    
    // Free the memory allocated for the matrix
    deallocate_matrix(matrix);
    
    MPI_Finalize();
    
    return 0;
}