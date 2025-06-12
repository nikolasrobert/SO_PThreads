#pragma once 
#define _CRT_SECURE_NO_WARNINGS 1  
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1 
#pragma comment(lib,"pthreadVC2.lib") 
#define HAVE_STRUCT_TIMESPEC 

#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>

#define MAX_MATRIX_ROWS 10
#define MAX_MATRIX_COLS 10
#define SEED 42

// Função para gerar número aleatório entre 0 e 31999
int randint() {
	return rand() % 32000;
}

// Função para alocar matriz dinamicamente
int** allocate_matrix() {
	int** matrix = (int**)malloc(sizeof(int*) * MAX_MATRIX_ROWS);
	if (matrix == NULL) {
		fprintf(stderr, "Erro ao alocar linhas da matriz.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		matrix[i] = (int*)malloc(MAX_MATRIX_COLS * sizeof(int));
		if (matrix[i] == NULL) {
			fprintf(stderr, "Erro ao alocar colunas da matriz na linha %d.\n", i);
			exit(EXIT_FAILURE);
		}
	}
	return matrix;
}

// Preenche a matriz com números aleatórios
int ehPrimo(int n) 
{
	int limit;
	if(limit = (int)sqrt((double)n))
	{
		for (int i = 1; i <= limit; i += 2) {
			if (n == 2) return 1;
			if (n % i == 0) return 0;
		}
	}
	else {
		return 0;
	}
}


void insert_matrix(int** matrix) {
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		for (int j = 0; j < MAX_MATRIX_COLS; j++) {
			matrix[i][j] = randint();
			int numero = matrix[i][j];
			// Descomente a linha abaixo para ver os valores
			printf("%d ", matrix[i][j]);
			if (ehPrimo(numero)==1)
			{
				int cont_primos = 0;
				cont_primos++;
			}
		}
		printf("\n"); // Para quebrar linha entre linhas da matriz
	}
}


int main(int argc, char* argv[]) {
	srand(SEED); // Define a semente do gerador de números aleatórios

	int** matrix = allocate_matrix();
	insert_matrix(matrix);

	// Libera a memória alocada
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		free(matrix[i]);
	}
	free(matrix);

	return 0;
}
