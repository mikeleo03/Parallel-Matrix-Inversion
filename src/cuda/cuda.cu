#include <iostream>
#include <cuda_runtime.h>

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

// Perform gaussian elimination
__global__ void gaussian_elimination_kernel(double *matrix, int size, int pivot) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < size && idx != pivot) {
        double ratio = matrix[idx * size * 2 + pivot] / matrix[pivot * size * 2 + pivot];
        for (int j = pivot; j < 2 * size; ++j) {
            matrix[idx * size * 2 + j] -= matrix[pivot * size * 2 + j] * ratio;
        }
    }
}

// Reduce the matrix to a unit matrix
__global__ void reduce_to_unit_kernel(double *matrix, int size, int pivot) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (idx < size && idx == pivot) {
        double pivot_value = matrix[idx * size * 2 + pivot];
        if (pivot_value != 0.0) {
            for (int j = idx; j < 2 * size; ++j) {
                matrix[idx * size * 2 + j] /= pivot_value;
            }
        }
    }
}

// Function to perform the matrix inversion in CUDA
void matrix_inversion_cuda(GaussianMatrix& matrix) {
    // Define and initiate value of the GPU matrix
    int size = matrix.size;
    double *d_matrix;

    // Allocate GPU memory for the matrix and copy the value
    cudaError_t err = cudaMalloc(&d_matrix, size * size * 2 * sizeof(double));
    if (err != cudaSuccess) {
        printf("%s in %s at line %d\n", cudaGetErrorString(err), __FILE__,__LINE__);
        exit(EXIT_FAILURE);
    }
    cudaMemcpy(d_matrix, matrix.mat, size * size * 2 * sizeof(double), cudaMemcpyHostToDevice);

    // Define the value of thread per block and block per grid
    int threadsPerBlock = 256;
    int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock;

    // Doing the paralel process on every pivot
    for (int pivot = 0; pivot < size; ++pivot) {
        // Do gaussian elimination
        gaussian_elimination_kernel<<<blocksPerGrid, threadsPerBlock>>>(d_matrix, size, pivot);
        cudaDeviceSynchronize();

        // Reduce the matrix to a unit matrix
        reduce_to_unit_kernel<<<blocksPerGrid, threadsPerBlock>>>(d_matrix, size, pivot);
        cudaDeviceSynchronize();
    }

    // Copy the result back to CPU memory
    cudaMemcpy(matrix.mat, d_matrix, size * size * 2 * sizeof(double), cudaMemcpyDeviceToHost);
    
    // Free the GPU memory
    cudaFree(d_matrix);
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
    
    // Initialize the matrix based on user input
    initialize_matrix(matrix);
    
    // Get the input matrix from user input
    input_matrix(matrix);

    // Perform matrix inversion in CUDA
    matrix_inversion_cuda(matrix);
    
    // Print the result
    print_result(matrix);

    // Free the memory allocated for the matrix
    deallocate_matrix(matrix);

    return 0;
}
