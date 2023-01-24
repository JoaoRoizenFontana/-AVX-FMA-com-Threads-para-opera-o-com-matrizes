/*
Joao Roizen Fontana - 1710431
Pedro Arduini - 2110132
*/

#include "matrix_lib.h"
#include <immintrin.h>
#include <pthread.h>

#define AVX_STEP 8


int num_global = 1;

 void set_number_threads(int num_threads){
  num_global = num_threads;
 }

void* rotina_scalar_matrix(void* args){
  
  struct Args1* argCerto =  (struct Args1*)args;
 
  float scalar_value = argCerto->scalar_value;
  
  __m256 vector, scalar, result;
  float * arrayNext = argCerto->rows;

  for(int i = 0; i < argCerto->tam; i+=AVX_STEP, arrayNext+=AVX_STEP){
    
    vector = _mm256_load_ps(arrayNext);
    scalar = _mm256_set1_ps(scalar_value);

    result = _mm256_mul_ps(vector, scalar);
      
    _mm256_store_ps(arrayNext, result);
  }
  
}

int scalar_matrix_mult(float scalar_value, struct matrix *matrix)
{
/* Multiplica matriz por um escalar usando avx. */
  if(matrix == NULL){
    printf("\nMatriz não declarada.\n");
    return 0;
  }

  int rc;
  pthread_t threads[num_global];
  struct Args1* vetorArgs[num_global];
  
  for(int i = 0;i<num_global;i++){
    vetorArgs[i] = (struct Args1*)malloc(sizeof(struct Args1));
  
    vetorArgs[i]->tam = (matrix->height*matrix->width)/num_global;
    vetorArgs[i]->rows = matrix->rows + (((matrix->height*matrix->width)/num_global)*i);
    vetorArgs[i]->scalar_value = scalar_value;
  }
  
  for(int i = 0;i<num_global;i++){

    rc = pthread_create(&threads[i],NULL,rotina_scalar_matrix,(void*)vetorArgs[i]);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      return 0;
    }
  }
    
  for(int i=0;i<num_global;i++){
    pthread_join(threads[i],NULL);
  }

  return 1;
}

void* rotina_matrix_matrix(void* args){
  struct Args2* argCerto = (struct Args2*)args;

  

  __m256 vectorA, vectorB, vectorC, result;

  float * arrayANext = argCerto->matrizA->rows;
  float * arrayBNext = argCerto->matrizB->rows;
  float * arrayCNext = argCerto->matrizC;
 
  for(int i = 0; i < (argCerto->matrizA->height*argCerto->matrizA->width); i++, arrayANext++){

    vectorA = _mm256_set1_ps(*arrayANext);
    arrayBNext = argCerto->matrizB->rows;

    int row = i / argCerto->matrizA->width;
    arrayCNext = argCerto->matrizC + row * argCerto->matrizB->width;

    for(int k = 0; k < argCerto->matrizB->width; k+=AVX_STEP, arrayBNext+=AVX_STEP, arrayCNext+=AVX_STEP){
      vectorB = _mm256_load_ps(arrayBNext);
      vectorC = _mm256_load_ps(arrayCNext);

      result = _mm256_fmadd_ps(vectorA, vectorB, vectorC);
      
      _mm256_store_ps(arrayCNext, result);
    }

  }


} 

int matrix_matrix_mult(struct matrix *matrix_a, struct matrix *matrix_b, struct matrix *matrix_c){ 
  if(matrix_a == NULL || matrix_b == NULL){
    printf("\nUma ou duas das matrizes não declaradas.\n");
    return 0;
  }

  if(matrix_a->width != matrix_b->height){
    printf("\nA matriz A deve ter o número de colunas igual ao número de linhas da matriz B.\n");
    return 0;
  }


  int rc;
  pthread_t threads[num_global];
  struct Args2* vetorArgs[num_global];
  
  for(int i = 0;i<num_global;i++){
    vetorArgs[i] = (struct Args2*)malloc(sizeof(struct Args2));

    vetorArgs[i]->matrizA = (struct matrix*)malloc(sizeof(struct matrix));
    vetorArgs[i]->matrizA->rows = matrix_a->rows + (((matrix_a->height/num_global)*matrix_a->width)*i);
    vetorArgs[i]->matrizA->height = matrix_a->height/num_global;
    vetorArgs[i]->matrizA->width = matrix_a->width;

    vetorArgs[i]->matrizB = (struct matrix*)malloc(sizeof(struct matrix));
    vetorArgs[i]->matrizB->rows = matrix_b->rows + (((matrix_b->height/num_global)*matrix_b->width)*i);
    vetorArgs[i]->matrizB->height = matrix_b->height/num_global;
    vetorArgs[i]->matrizB->width = matrix_b->width;

    vetorArgs[i]->matrizC = matrix_c->rows + (((matrix_b->height*matrix_b->width)/num_global)*i);

  }
  
  for(int i = 0;i<num_global;i++){

    rc = pthread_create(&threads[i],NULL,rotina_matrix_matrix,(void*)vetorArgs[i]);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      return 0;
    }
  }
    
  for(int i=0;i<num_global;i++){
    pthread_join(threads[i],NULL);
  }

  
  return 1;
}
