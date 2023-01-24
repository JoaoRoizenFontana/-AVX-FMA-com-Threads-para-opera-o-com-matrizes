#include <stdio.h>
#include <stdlib.h>


struct matrix {
unsigned long int height;
unsigned long int width;
float *rows;
};

struct Args1{
  float scalar_value;
  long int tam;
  float* rows;
};

struct Args2{
  struct matrix* matrizA;
  struct matrix* matrizB;
  float* matrizC;
};

void set_number_threads(int num_threads);

void* rotina_scalar_matrix(void* args);

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC);
