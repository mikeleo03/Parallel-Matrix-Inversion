#include <iostream>
#include <mpi.h>

using namespace std;

struct GaussianMatrix {
    int size;
    double *mat;
};

// Initialize the matrix based on user input
void initialize_matrix(GaussianMatrix& matrix, int& local_n_row, int my_rank, int num_procs) {
    if (my_rank == 0) {
        cin >> matrix.size;
        local_n_row = matrix.size / num_procs;
    }
    // Broadcast the number of rows per process
    MPI_Bcast(&matrix.size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&local_n_row, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the matrix
    if (my_rank == 0) {
        // Root process has complete matrix
        matrix.mat = new double[matrix.size * matrix.size * 2];
    } else {
        // Other processes has partial matrix
        matrix.mat = new double[local_n_row * matrix.size * 2];
    }
}

// Function to get the input matrix from user input and init identity
void input_matrix(GaussianMatrix& matrix, int local_n_row, int my_rank, int num_procs) {
    if (my_rank == 0) {
        // Get the user input
        for (int i = 0; i < matrix.size; ++i) {
            for (int j = 0; j < matrix.size; ++j) {
                cin >> matrix.mat[i * matrix.size * 2 + j];
            }
            // Initialize right-hand side to identity matrix
            for (int j=matrix.size; j < matrix.size * 2; ++j) {
                if (j == i + matrix.size) {
                    matrix.mat[i * matrix.size * 2 + j] = 1;
                } else {
                    matrix.mat[i * matrix.size * 2 + j] = 0;
                }
            }
        }
    }

    // Broadcast the parts of the input matrix
    MPI_Scatter(matrix.mat, local_n_row * matrix.size * 2, MPI_DOUBLE, 
                matrix.mat, local_n_row * matrix.size * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

// Free the memory allocated for the matrix
void deallocate_matrix(GaussianMatrix& matrix) {
    delete[] matrix.mat;
}

// Inverse the matrix
void parallel_inverse_matrix(GaussianMatrix& matrix, int local_n_row, int my_rank, int num_procs) {
    int i, j;
    double d;
    int n = matrix.size;

    // Perform pivoting locally within each process
    // Set actual/global row index range held by object
    int startIdx = my_rank * local_n_row;
    int endIdx = startIdx + local_n_row - 1;
    int* pivot_rows_indexes;
    double* pivot_rows;
    if (my_rank == 0) {
        pivot_rows_indexes = new int[num_procs];
        pivot_rows = new double[n * 2 * num_procs];
    }
    for (i = 0; i < n; ++i) {
        int adjusted_pivot_row_idx = -1;
        double *pivot_row = new double[n * 2];
        // Search pivot only if process has the needed row(s)
        double pivot_value = 0;
        double *row_i = new double[n * 2];
        if (i <= endIdx) {
            if (i >= startIdx) { // This process has row i
                for (j = 0; j < 2 * n; ++j) { // Set row i
                    row_i[j] = matrix.mat[(i-startIdx) * n * 2 + j];
                }
                // Send row i to root process if row i is not in root
                if (my_rank != 0) {
                    MPI_Send(row_i, n * 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
                }
            }
            int pivot_row_idx = max(0, i-startIdx);
            pivot_value = matrix.mat[pivot_row_idx * n * 2 + pivot_row_idx];

            // Find the row with the maximum value in the current column
            for (int row = pivot_row_idx + 1; row < local_n_row; ++row) {
                if (abs(matrix.mat[row * n * 2 + row]) > abs(pivot_value)) {
                    pivot_row_idx = row;
                    pivot_value = matrix.mat[row * n * 2 + i];
                }
            }
            adjusted_pivot_row_idx = pivot_row_idx + startIdx;
            for (int k=0; k<n*2; ++k) {
                pivot_row[k] = matrix.mat[pivot_row_idx * n * 2 + k];
            }
        }
        // Calculate which process has row i
        int i_copy = i;
        int row_i_root = 0;
        while (i_copy > local_n_row-1) {
            i_copy -= local_n_row;
            row_i_root++;
        }
        if (my_rank == 0 && row_i_root != 0) {
            // Receive row i to be broadcasted
            MPI_Recv(row_i, n * 2, MPI_DOUBLE, row_i_root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        // Send pivot row and its index to root (process 0)
        MPI_Gather(&adjusted_pivot_row_idx, 1, MPI_INT, pivot_rows_indexes, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Gather(pivot_row, n * 2, MPI_DOUBLE, pivot_rows, n * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        double *rescaled_pivot_row = new double[n * 2];
        if (my_rank == 0) {
            // Search best pivot from all received pivots
            for (int k = 0; k < num_procs; ++k) {
                if (abs(pivot_rows[k * n * 2 + i]) > abs(pivot_value) && pivot_rows_indexes[k] >= 0) {
                    adjusted_pivot_row_idx = pivot_rows_indexes[k];
                    pivot_value = pivot_rows[k * n * 2 + i];
                    for (int l=0; l<n*2; ++l) {
                        pivot_row[l] = pivot_rows[k * n * 2 + l];
                    }
                }
            }
            // Divide best pivot row by leading coefficient
            for (int k = 0; k < n * 2; ++k) {
                rescaled_pivot_row[k] = pivot_row[k]/pivot_value;
            }
        }
        // Broadcast pivot row to all processes
        MPI_Bcast(&adjusted_pivot_row_idx, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(rescaled_pivot_row, n * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(row_i, n*2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        if (my_rank == row_i_root) {
            // Swap row i with rescaled pivot row
            for (j = 0; j < 2 * n; ++j) {
                matrix.mat[(i-startIdx) * n * 2 + j] = rescaled_pivot_row[j];
            }
        }

        if (adjusted_pivot_row_idx != i && adjusted_pivot_row_idx >= startIdx && adjusted_pivot_row_idx <= endIdx) {
            for (j = 0; j < 2 * n; ++j) {
                matrix.mat[(adjusted_pivot_row_idx-startIdx) * n * 2 + j] = row_i[j];
            }
        }

        // Subtract all non-diagonal values in column i to zero
        for (int row = 0; row < local_n_row; ++row) {
            if (row + startIdx != i) {
                d = matrix.mat[row * 2 * n + i];
                for (int col = 0; col < 2 * n; ++col) {
                    matrix.mat[row * 2 * n + col] -= rescaled_pivot_row[col] * d;
                }
            }
        }
        delete[] row_i;
        delete[] pivot_row;
    }
    
    // Combine all matrix
    if (my_rank == 0) {
        MPI_Gather(matrix.mat, local_n_row * matrix.size * 2, MPI_DOUBLE, matrix.mat, local_n_row * matrix.size * 2, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    } else {
        MPI_Gather(matrix.mat, local_n_row * matrix.size * 2, MPI_DOUBLE, NULL, local_n_row * matrix.size * 2, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);
    }
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
    int local_n_row;
    
    // Initialize the matrix based on user input
    initialize_matrix(matrix, local_n_row, my_rank, num_procs);
    
    // Get the input matrix from user input
    input_matrix(matrix, local_n_row, my_rank, num_procs);
    
    // Perform partial pivoting
    parallel_inverse_matrix(matrix, local_n_row, my_rank, num_procs);
    
    // Print the result
    print_result(matrix, my_rank);
    
    // Free the memory allocated for the matrix
    deallocate_matrix(matrix);
    
    MPI_Finalize();
    
    return 0;
}