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
#define MAX_ROWS_MACROBLOCK 100
#define MAX_COLS_MACROBLOCK 100


//define paralelo
#define BLOCK_H    100      // altura do macrobloco (ajuste nos testes)
#define BLOCK_W    100      // largura do macrobloco
#define N_THREADS  8        // número de threads padrão (será parâmetro em main)


// Funcao para gerar numero aleatorio entre 0 e 31999
int randint() {
	return rand() % 32000;
}

// Funcao para alocar matriz dinamicamente
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


//variáveis para paralelo

int next_block = 0;
long total_primos = 0;
pthread_mutex_t mtx_block;
pthread_mutex_t mtx_count;
int total_blocks;
int n_blocks_row, n_blocks_col;


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
			//printf("%d ", matrix[i][j]);
		}
		printf("\n"); // Para quebrar linha entre linhas da matriz
	}
}

//adicione tempo na fun��o de busca

void serial_search(int** matrix) {
	double serial_start_time = (double)clock() / CLOCKS_PER_SEC; // Inicia o cron�metro
	int cont_primos = 0;
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		for (int j = 0; j < MAX_MATRIX_COLS; j++) {
			if (ehPrimo(matrix[i][j]) == 1) {
				cont_primos++;
			}
		}
	}
	double serial_end_time = (double)clock() / CLOCKS_PER_SEC; // Para o cron�metro
	printf("\n * * Matrix Size : %d x %d * *\n", MAX_MATRIX_COLS, MAX_MATRIX_ROWS);
	printf("Busca serial concluida.\n");
	printf("\nTempo de execucao serial: %.2f segundos\n", serial_end_time - serial_start_time);
	printf("Total de numeros primos encontrados: %d\n", cont_primos);

}

void print_matrix(int** matrix)
{
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		for (int j = 0; j < MAX_MATRIX_COLS; j++) {
			// Descomente a linha abaixo para ver os valores
			printf("%d ", matrix[i][j]);
		}
		printf("\n"); // Para quebrar linha entre linhas da matriz
	}
}

/* **********  MENU  ********** */
void menu(int **matrix) {
    int option;
    
    do {
        printf("\n---------------- MENU -------------------\n");
        printf("1. Print Matrix\n");
        printf("2. Print Matrix's Blocks\n");
        printf("3. Count Prime Numbers (Serial)\n");
        printf("4. Count Prime Numbers (Parallel)\n");
        printf("5. Calculate Speedup (Runs both counters)\n");
        printf("0. Exit\n");
        printf("-----------------------------------------\n");

        if (scanf("%d", &option)) {
            system("cls || clear");
            printf("\n\n");
        }
        else {
            printf("** Error: Invalid option!!! **\n**  Please enter a valid option **\n");
            break;
        }

        switch (option)
        {
        case 1:
            printf("** Printing the full matrix...         **\n");
            printf("-----------------------------------------\n");
		
			printf("** Matrix filled with random numbers... **\n");
			print_matrix(matrix);

            break;

        case 2:
            printf("** Printing each block in matrix...    **\n");
            printf("-----------------------------------------\n");


            break;

        case 3:
            /* SERIAL PRIME COUNTING TEST */
			serial_search(matrix);
			printf("** Running Serial Count...             **\n");
			printf("-----------------------------------------\n");
			printf("** Run time: %.6f seconds          **\n", 0.0); // Placeholder for run time
			break;
		case 4:
			/* PARALLEL PRIME COUNTING TEST */
			printf("** Running Parallel Count...           **\n");
			printf("-----------------------------------------\n");
			printf("** Run time: %.6f seconds          **\n", 0.0); // Placeholder for run time
			break;
		case 5:
			/* SPEEDUP CALCULATION TEST */
			printf("** Calculating Speedup...              **\n");
			printf("-----------------------------------------\n");
			printf("** Running Serial Count...             **\n");
            printf("-----------------------------------------\n");
            //printf("** Run time: %.6f seconds          **\n");

            break;

       
        default:
            printf("** Error: Invalid option!!! **\n**  Please enter a valid option **\n");
        }
    } while (option);

}

int main(int argc, char* argv[]) {
	// variaveis paralelo
	n_blocks_row = (N_ROWS + BLOCK_H - 1) / BLOCK_H;
	n_blocks_col = (N_COLS + BLOCK_W - 1) / BLOCK_W;
	total_blocks = n_blocks_row * n_blocks_col;

	pthread_mutex_init(&mtx_block, NULL);
	pthread_mutex_init(&mtx_count, NULL);

	srand(SEED); // Define a semente do gerador de n�meros aleat�rios

	int** matrix = allocate_matrix();
	insert_matrix(matrix);
	
	menu(matrix);	

	 //Libera a mem�ria alocada
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		free(matrix[i]);
	}
	free(matrix);

	return 0;
}