#include "timer.h"
#include <stdio.h>
#include "matrix_lib.h"





int main(int argc, char *argv[]){
	struct timeval start, stop, t1, t0;
	struct matrix A, B, C;
	FILE* matrizA;
	FILE* matrizB;
	FILE* resultado1;
	FILE* resultado2;
	int w, rA, cA, rB, cB, num_threads;
	float *auxA;
	float *auxB;
	float *auxC;
	float scalar;

	gettimeofday(&start,NULL);
	
	matrizA = fopen(argv[7],"rb");
	matrizB = fopen(argv[8],"rb");
	
	rA = atoi(argv[2]);
	cA = atoi(argv[3]);
	rB = atoi(argv[4]);
	cB = atoi(argv[5]);
  num_threads = atoi(argv[6]);
	
	printf("Matrix A - \n");
	auxA = (float*)aligned_alloc(32,(rA*cA)*sizeof(float));
	for(w=0;w<(rA*cA);w++){
		fread(auxA,sizeof(float),1,matrizA);
		if(w<256){
			printf("%1.f ",*auxA);
		}
		if(w==256){
                        printf("..Já foram 256\n");
                }
		auxA++;
	}
	
	A.rows = auxA-w;
	
	printf("Matrix B - \n");
	auxB = (float*)aligned_alloc(32,(rB*cB)*sizeof(float));
        for(w=0;w<(rB*cB);w++){
                fread(auxB,sizeof(float),1,matrizB);
		if(w<256){
                        printf("%1.f ",*auxB);
                }
		if(w==256){
			printf("..Já foram 256\n");
		}
                auxB++;
        }
        B.rows = auxB-w;
	        

	auxC = (float*)aligned_alloc(32,(rA*cB)*sizeof(float));
        for(w=0;w<(rA*cB);w++){
                *auxC = 0;
                auxC++;
        }

	A.height = rA;
	A.width = cA;
	
	B.height = rB;
	B.width = cB;
	

	C.height = rA;
	C.width = cB;
  C.rows = auxC-w;

  set_number_threads(num_threads);
  
	scalar = atof(argv[1]);
	resultado1 = fopen(argv[9],"wb");
        gettimeofday(&t1,NULL);
	scalar_matrix_mult(scalar,&A);	
	gettimeofday(&t0,NULL);
	printf("Tempo função 1: %f ms\n", timedifference_msec(t1,t0));
	
	auxA = A.rows;
	printf("Matrix A (depois da função) -\n");
	for(w=0;w<(rA*cA);w++){
		fwrite(auxA,sizeof(float),1,resultado1);
		if(w<256){
                        printf("%1.f ",*auxA);
                }
		if(w==256){
                        printf("..Já foram 256\n");
                }
		auxA++;
	}

	resultado2 = fopen(argv[10],"wb");
	gettimeofday(&t1,NULL);
	matrix_matrix_mult(&A,&B,&C);
	gettimeofday(&t0,NULL);
	printf("Tempo função 2: %f ms\n", timedifference_msec(t1,t0));

	auxC = C.rows;
	printf("Matriz C -\n");
	for(w=0;w<(rA*cB);w++){
		fwrite(auxC,sizeof(float),1,resultado2);
		if(w<256){
                        printf("%1.f ",*auxC);
                }
                if(w==256){
                        printf("..Já foram 256\n");
                }
		auxC++;
	}
	
	fclose(matrizA);
	fclose(matrizB);
	fclose(resultado1);
	fclose(resultado2);

	gettimeofday(&stop,NULL);
	printf("Tempo total: %f ms\n", timedifference_msec(start,stop));

	return 0;
}

