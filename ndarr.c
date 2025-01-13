#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    float *data;
    int *shape;
    int *strides;
    int size;
    int ndim;
} Array;


Array *createArray(int *shape, int ndim)
{
    // Allocate memory for array
    Array *array = (Array *)malloc(sizeof(Array));
    if (!array) {
        perror("Failure to allocate memory for array");
        exit(EXIT_FAILURE);
    }

    // Calculate strides
    array->strides = (int *)malloc(ndim * sizeof(int));
    int s = 1;
    for (int i = ndim-1; i >= 0; i--) {
        array->strides[i] = s;
        s *= shape[i];
    }

    // Calculate size and allocate size
    int size = 1;
    for (int i = 0; i < ndim; i++) 
        size *= shape[i];
    array->size = size;

    // Initialize other array properties
    array->data = (float *)malloc(size * sizeof(float));
    if (!array->data) {
        free(array);
        perror("Failure to allocate memory for array");
        exit(EXIT_FAILURE);
    }
    
    
    array->shape = shape;
    array->ndim = ndim;

    return array;
}

void addArrays(Array *a, Array *b, Array *result)
{
    if (a->ndim == b->ndim) {
        if (a->size == b->size) {
            for (int i = 0; i < a->size; i++) {
                result->data[i] = a->data[i] + b->data[i];
            }
        } else {
             // Handle Broadcasting
             if (b->size == 1) {
                for (int i = 0; i < a->size; i++)
                    result->data[i] = a->data[i] + b->data[0];
             } else if (a->size == 1) {
                for (int i = 0; i < b->size; i++) 
                    result->data[i] = a->data[0] + b->data[i];
             }
        } 
    } else {
        // If sizes do not match and no broadcasting applies
        fprintf(stderr, "Error: Arrays are not broadcastable or incompatible sizes.\n");
        return;
    }
}

void randInit(Array *array, int low, int high)
{
    for (int i = 0; i < array->size; i++)
        array->data[i] = low + rand() % (high - low + 1);
}

void zeroInit(Array *array)
{
    for (int i = 0; i < array->size; i++) 
        array->data[i] = 0.0;
}

void matmul(Array *a, Array *b, Array *res)
{
    if (a->ndim < 2 || b->ndim < 2) {
        perror("Both arrays must have at least 2 dimensions for matrix multiplication");
        exit(EXIT_FAILURE);
    }

    // Check if last dim in a is equal to the second-last dim in b
    if (a->shape[a->ndim - 1] != b->shape[b->ndim -2]) {
        perror("Last dim of first array must match second-last dim of second array");
        exit(EXIT_FAILURE);
    }

    int row = a->shape[a->ndim - 2];
    int col = b->shape[b->ndim - 2];
    int inner = a->shape[a->ndim - 1];

    // Assumption is that Array *res has been checked to ensure the right shape
    if (a->ndim == 2 && b->ndim == 2) {
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                float tmp = 0.0;
                for (int k = 0; k < inner; k++) {
                    int idx_a = i * a->strides[0] + k * a->strides[1];
                    int idx_b = k * a->strides[0] + j * b->strides[1];
                    tmp += a->data[idx_a] * b->data[idx_b];
                }
                int idx_res = i * res->strides[0] + j * res->strides[1];
                res->data[idx_res] = tmp;
            }
        }
    }

    // TODO: Add code for ndim > 2
}

void printArrayRecursive(Array *array, int *indices, int ndim, int dimIndex) 
{
    // Base case: if we reached the last dimension, print the element
    if (dimIndex == ndim - 1) {
        for (int i = 0; i < array->shape[dimIndex]; i++) {
            indices[dimIndex] = i;
            int index = 0;
            for (int j = 0; j < ndim; j++) {
                index += indices[j] * array->strides[j];
            }
            printf("%.1f ", array->data[index]);
        }
        printf("\n");
        return;
    }

    // Recursive case: traverse the current dimension
    for (int i = 0; i < array->shape[dimIndex]; i++) {
        indices[dimIndex] = i;
        printArrayRecursive(array, indices, ndim, dimIndex + 1);
        if (dimIndex == 0) { // After each top-level "row" of first dim, print an extra newline
            printf("\n");
        }
    }
}

void printArray(Array *array)
{
    int *indices = (int *)malloc(array->ndim * sizeof(int));
    if (!indices) {
        perror("Failure to allocate memory for indices");
        return;
    }

    printArrayRecursive(array, indices, array->ndim, 0);

    free(indices);
}

int main() 
{
    srand(time(NULL));

    int shape[3] = {2, 3, 4};
    int ndim = 3;
    
    // Test Init
    Array *arr = createArray(shape, ndim);
    randInit(arr, 0, 4);

    // Addition Test 1: Same dimensions
    Array *a = createArray(shape, ndim);
    Array *b = createArray(shape, ndim);
    Array *result = createArray(shape, ndim);

    randInit(a, 2, 2);   // Initialize every element to be 2
    randInit(b, 3, 3);   // Initialize every element to be 3
    zeroInit(result);

    addArrays(a, b, result);
    
    // Debugging purpose
    for (int i = 0; i < result->size; i++) {
        printf("%.1f ", result->data[i]);
    }
    printf("\n");
    printf("\n");

    printArray(result);

    free(arr->data);
    free(arr->strides);
    free(arr);

}


