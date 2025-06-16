#define _CRT_SECURE_NO_WARNINGS 1  
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1 
#pragma comment(lib,"pthreadVC2.lib") 
#define HAVE_STRUCT_TIMESPEC 

#include "pthread.h"
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
int BLOCK_H = 100;      // altura do macrobloco (ajuste nos testes)
int BLOCK_W = 100;      // largura do macrobloco
#define N_THREADS  8        // número de threads padrão (será parâmetro em main)

int threads  = N_THREADS; 

int **matrix;

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

	// se nao for encontrado divisores, é primo
	return 1;
}

//funcao trabalho da thread
void *thread_func(void *arg) {
    while (1) {
        pthread_mutex_lock(&mtx_block);
        if (next_block >= total_blocks) {
            pthread_mutex_unlock(&mtx_block);
            break;
        }
        int blk = next_block++;
        pthread_mutex_unlock(&mtx_block);

        int br = blk / n_blocks_col;
        int bc = blk % n_blocks_col;
        int rs = br * BLOCK_H;
        int cs = bc * BLOCK_W;
        int re = rs + BLOCK_H;
        int ce = cs + BLOCK_W;

        if (re > MAX_MATRIX_ROWS) re = MAX_MATRIX_ROWS;
		if (ce > MAX_MATRIX_COLS) ce = MAX_MATRIX_COLS;


        long local_count = 0;
        for (int i = rs; i < re; i++)
            for (int j = cs; j < ce; j++)
                if (ehPrimo(matrix[i][j]))
                    local_count++;

        pthread_mutex_lock(&mtx_count);
        total_primos += local_count;
        pthread_mutex_unlock(&mtx_count);
    }
    return NULL;
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

//adicione tempo na funcao de busca

// Realiza busca serial, imprime resultados e retorna o tempo em segundos
double serial_search(int** matrix) {
    double t0 = (double)clock() / CLOCKS_PER_SEC;

    int cont_primos = 0;
    for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
        for (int j = 0; j < MAX_MATRIX_COLS; j++) {
            if (ehPrimo(matrix[i][j])) {
                cont_primos++;
            }
        }
    }

    double t1 = (double)clock() / CLOCKS_PER_SEC;
    double elapsed = t1 - t0;

    printf("\n * * Matrix Size : %d x %d * *\n", MAX_MATRIX_COLS, MAX_MATRIX_ROWS);
    printf("Busca serial concluída.\n");
    printf("Tempo de execução serial: %.6f segundos\n", elapsed);
    printf("Total de números primos encontrados: %d\n", cont_primos);

    return elapsed;
}

//Realiz busca paralela

double parallel_search(int threads) {
    next_block   = 0;
    total_primos = 0;

    pthread_mutex_init(&mtx_block, NULL);
    pthread_mutex_init(&mtx_count, NULL);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    pthread_t tids[threads];
    for (int t = 0; t < threads; t++) {
        pthread_create(&tids[t], NULL, thread_func, NULL);
    }
    for (int t = 0; t < threads; t++) {
        pthread_join(tids[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double tp = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    pthread_mutex_destroy(&mtx_block);
    pthread_mutex_destroy(&mtx_count);

    printf("Parallel: threads=%d, bloco=%dx%d → primos=%ld, tempo=%.6fs\n",
           threads, BLOCK_H, BLOCK_W, total_primos, tp);

    return tp;
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
            /* SERIAL COUNTING TEST */
			printf("** Running Serial Count...             **\n");
			printf("-----------------------------------------\n");
			double t = serial_search(matrix);
      		printf("-> Tempo serial: %.6f s\n", t);
			break;

		case 4:
			/* PARALLEL COUNTING TEST */
			printf("** Running Parallel Count...           **\n");
			printf("-----------------------------------------\n");
			double tp = parallel_search(threads);
  			printf("-> Tempo paralelo: %.6f s\n", tp);
			break;
		case 5:
			/* SPEEDUP CALCULATION TEST */
			printf("** Calculating Speedup...              **\n");
			printf("-----------------------------------------\n");
			printf("** Running Serial Count...             **\n");
            printf("-----------------------------------------\n");
			double ts = serial_search(matrix);
			double tp_par = parallel_search(threads);
			printf("-> Speedup: %.2fx\n", ts/tp_par);
            

            break;

       
        default:
            printf("** Error: Invalid option!!! **\n**  Please enter a valid option **\n");
        }
    } while (option);

}

int main(int argc, char* argv[]) {
	
	// criação e join das threads
	
	if (argc >= 4) {
		threads  = atoi(argv[1]);
		BLOCK_H  = atoi(argv[2]);
		BLOCK_W  = atoi(argv[3]);
	};
	
	// variaveis paralelo
	n_blocks_row = (MAX_MATRIX_ROWS + BLOCK_H - 1) / BLOCK_H;
	n_blocks_col = (MAX_MATRIX_COLS + BLOCK_W - 1) / BLOCK_W;

	total_blocks = n_blocks_row * n_blocks_col;

	pthread_mutex_init(&mtx_block, NULL);
	pthread_mutex_init(&mtx_count, NULL);

	srand(SEED); // Define a semente do gerador de numeros aleatorios

	// alocar e preencher a matriz ANTES de iniciar as threads
	matrix = allocate_matrix();
	insert_matrix(matrix);

	

	
	menu(matrix);	

	 //Libera a memoria alocada
	for (int i = 0; i < MAX_MATRIX_ROWS; i++) {
		free(matrix[i]);
	}
	free(matrix);

	//destruir mutex
	pthread_mutex_destroy(&mtx_block);
	pthread_mutex_destroy(&mtx_count);


	return 0;
}