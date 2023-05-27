#pragma once
#include <stdlib.h>
#include <stdio.h>

size_t allocated_memory = 0;
size_t allocation_count = 0;

void *checked_malloc(size_t size)
{
    allocation_count += 1;
    allocated_memory += size;
    return malloc(size);
}

void *checked_calloc(size_t count, size_t size)
{
    allocation_count += 1;
    allocated_memory += size * count;
    return calloc(count, size);
}

void checked_free(void *ptr)
{
    allocation_count -= 1;
    free(ptr);
}

void check_memory(unsigned int rows, unsigned int cols)
{
    if (allocation_count == 0)
        printf("PASS: allocation count\n");
    else
        printf("FAIL: allocation count\n");

    unsigned int total_memory = rows * sizeof(double *) + rows * cols * sizeof(double);

    if (allocated_memory == total_memory)
        printf("PASS: allocation size\n");
    else
        printf("FAIL: allocation size\n");
}

#define malloc(X) checked_malloc(X)
#define calloc(X, Y) checked_calloc(X, Y)
#define free(X) checked_free(X)

void check_matrix(double **matrix, unsigned int rows, unsigned int cols)
{
    for (unsigned int i = 0; i < rows; i++)
    {
        for (unsigned int j = 0; j < cols; j++)
        {
            matrix[i][j] = rand() % 100 + 1.0;
        }
    }
}