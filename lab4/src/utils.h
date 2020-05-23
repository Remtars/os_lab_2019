#ifndef UTILS_H
#define UTILS_H

struct MinMax {
  int min;
  int max;
};

void GenerateArray(int *array, unsigned int array_size, unsigned int seed);
void PrintArray(int *array, unsigned int array_size);
void GenerateArrayModulo(int *array, unsigned int array_size, unsigned int seed, unsigned int modulo);

#endif
