#include <iostream>
#include <mpi.h>

using namespace std;

struct GaussianMatrix {
    int size;
    double **mat;
};

// Initialize the matrix based on user input
void initialize_matrix(GaussianMatrix& matrix, int my_rank, int num_procs) {
    if (my_rank == 0) {
        cin >> matrix.size;
    }
    
    // Broadcast the size of the matrix to all processes
    MPI_Bcast(&matrix.size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the matrix
    matrix.mat = new double*[2 * matrix.size];
    for (int i = 0; i < 2 * matrix.size; ++i) {
        matrix.mat[i] = new double[2 * matrix.size]();
    }
}

// Function to get the input matrix from user input and init identity
void input_matrix(GaussianMatrix& matrix) {
    // Get the user input
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = 0; j < matrix.size; ++j) {
            cin >> matrix.mat[i][j];
        }
    }

    // Initialize right-hand side to identity matrix
    for (int i = 0; i < matrix.size; i++) {
        for (int j = 0; j < 2 * matrix.size; j++) {
            if (j == (i + matrix.size)) {
                matrix.mat[i][j] = 1;
            }
        }
    }
}

// Free the memory allocated for the matrix
void deallocate_matrix(GaussianMatrix& matrix) {
    for (int i = 0; i < matrix.size; ++i) {
        delete[] matrix.mat[i];
    }
    delete[] matrix.mat;
}

// Perform partial pivoting
void parallel_partial_pivot(GaussianMatrix& matrix, int my_rank, int num_procs) {
    int i, j;
    double d;
    int n = matrix.size;

    // Calculate the chunk size for each process
    int chunk_size = n / num_procs;
    
    // Determine the starting and ending indices for the current process
    int start_index = my_rank * chunk_size;
    int end_index = (my_rank == num_procs - 1) ? n : (start_index + chunk_size);
    
    // Perform pivoting locally within each process
    for(i = n; i > 1; --i) {
        if(matrix.mat[i-1][1] < matrix.mat[i][1]) {
            for(j = 0; j < 2 * n; ++j) {
                d = matrix.mat[i][j];
                matrix.mat[i][j] = matrix.mat[i-1][j];
                matrix.mat[i-1][j] = d;
            }
        }
    }
    
    // Gather results from all processes and synchronize
    MPI_Barrier(MPI_COMM_WORLD);
}

// Reduce the matrix to a diagonal form
void parallel_reduce_to_diagonal(GaussianMatrix& matrix, int my_rank, int num_procs) {
    int i, j, k;
    double d;
    int n = matrix.size;

    // Calculate the chunk size for each process
    int chunk_size = n / num_procs;
    
    // Determine the starting and ending indices for the current process
    int start_index = my_rank * chunk_size;
    int end_index = (my_rank == num_procs - 1) ? n : (start_index + chunk_size);
    
    // Reduce to diagonal matrix
    for (i = 0; i < n; ++i) {
        for (j = 0; j < 2 * n; ++j) {
            if (j != i) {
                d = matrix.mat[j][i] / matrix.mat[i][i];
                for (k = 0; k < 2 * n; ++k) {
                    matrix.mat[j][k] -= matrix.mat[i][k] * d;
                }
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

    // Calculate the chunk size for each process
    int chunk_size = n / num_procs;
    
    // Determine the starting and ending indices for the current process
    int start_index = my_rank * chunk_size;
    int end_index = (my_rank == num_procs - 1) ? n : (start_index + chunk_size);
    
    // Reduce to unit matrix locally within each process
    for (i = 0; i < n; ++i) {
        d = matrix.mat[i][i];
        for (j = 0; j < 2 * n; ++j) {
            matrix.mat[i][j] /= d;
        }
    }
    
    // Synchronize all processes before proceeding
    MPI_Barrier(MPI_COMM_WORLD);
}

// Function to print the resulting matrix
void print_result(GaussianMatrix& matrix) {
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = matrix.size; j < 2 * matrix.size; ++j) {
            cout << matrix.mat[i][j] << " ";
        }
        cout << endl;
    }
}

// [DEBUG] Function to print lefthand side
void print_result_left(GaussianMatrix& matrix) {
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = 0; j < matrix.size; ++j) {
            cout << matrix.mat[i][j] << " ";
        }
        cout << endl;
    }
}

// [DEBUG] Function to print all the matrix elements generated
void print_matrix(GaussianMatrix& matrix, int my_rank) {
    cout << "Result Matrix (Process " << my_rank << "):" << endl;
    for (int i = 0; i < matrix.size; ++i) {
        for (int j = 0; j < matrix.size; ++j) {
            cout << matrix.mat[i][j] << " ";
        }
        cout << endl;
    }
}

// Doing the whole matrix process
void process_matrix(GaussianMatrix& matrix, int my_rank, int num_procs) {
    // Perform partial pivoting
    parallel_partial_pivot(matrix, my_rank, num_procs);
    
    // Synchronize all processes before proceeding
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Reduce the matrix to a diagonal form
    parallel_reduce_to_diagonal(matrix, my_rank, num_procs);
    
    // Synchronize all processes before proceeding
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Reduce the matrix to a unit matrix
    parallel_reduce_to_unit(matrix, my_rank, num_procs);
    
    // Synchronize all processes before printing the result
    MPI_Barrier(MPI_COMM_WORLD);
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
    input_matrix(matrix);
    
    // Doing the whole matrix process
    process_matrix(matrix, my_rank, num_procs);
    
    // Print the result
    if (my_rank == 0) {
        cout << matrix.size << endl;
        print_result(matrix);
    }
    
    // Free the memory allocated for the matrix
    deallocate_matrix(matrix);
    
    MPI_Finalize();
    
    return 0;
}
