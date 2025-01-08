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

void randInit(Array *array, int low, int high)
{
    for (int i = 0; i < array->size; i++)
        array->data[i] = low + rand() % (high - low + 1);
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

    Array *arr = createArray(shape, ndim);
    randInit(arr, 0, 4);
    
    /** Debugging purpose
    for (int i = 0; i < arr->size; i++) {
        printf("%.1f ", arr->data[i]);
    }
    printf("\n");
    printf("\n");
    **/

    printArray(arr);

    free(arr->data);
    free(arr->strides);
    free(arr);

}


