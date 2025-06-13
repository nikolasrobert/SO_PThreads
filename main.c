#pragma once 
#define _CRT_SECURE_NO_WARNINGS 1  
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1 
#pragma comment(lib,"pthreadVC2.lib") 
#define HAVE_STRUCT_TIMESPEC 

#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <time.h>

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


int ehPrimo(int number) {
	// if the number is less than or equal to 1, it is not prime.
	if (number <= 1) {
		return 0;
	}

	int square_root = (int)sqrt(number);
	// Check for divisors from 2 to the square root of the number.
	// If any divisor is found, the number is not prime.
	for (int i = 2; i <= square_root; i++) {
		if (number % i == 0) {
			return 0;
		}
	}

	// If no divisors are found, the number is prime.
	return 1;
}

void insert_matrix(int** matrix) {
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		for (int j = 0; j < MAX_MATRIX_COLS; j++) {
			matrix[i][j] = randint();
			int numero = matrix[i][j];
			// Descomente a linha abaixo para ver os valores
			printf("%d ", matrix[i][j]);
		}
		printf("\n"); // Para quebrar linha entre linhas da matriz
	}
}


void serial_search(int** matrix) {
	int cont_primos = 0;
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		for (int j = 0; j < MAX_MATRIX_COLS; j++) {
			if (ehPrimo(matrix[i][j]) == 1) {
				printf(" antes: %d", cont_primos);
				cont_primos++;
				printf("\ndepois: %d", cont_primos);
			}
		}
	}

}

int main(int argc, char* argv[]) {
	srand(SEED); // Define a semente do gerador de números aleatórios

	int** matrix = allocate_matrix();
	insert_matrix(matrix);
	serial_search(matrix);
	


	// Libera a memória alocada
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		free(matrix[i]);
	}
	free(matrix);

	return 0;
}
