#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
void GenerateArray(int *array, unsigned int array_size, unsigned int seed) 
{
    srand(seed);
    for (int i = 0; i < array_size; i++) 
    {
        array[i] = rand();
    }
}

void GenerateArrayModulo(int *array, unsigned int array_size, unsigned int seed, unsigned int modulo) 
{
    srand(seed);
    for (int i = 0; i < array_size; i++) 
    {
        array[i] = rand() % modulo;
    }
}

void PrintArray(int *array, unsigned int array_size)
{
    printf("Array:\n");
    for (int i = 0; i < array_size; i++) printf("%d ", array[i]);
    printf("\n");
}