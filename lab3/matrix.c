#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

/*
    Uso do programa: ./out <matriz1> <matriz2> <saida> <n_threads> <flag>
    Flags disponíveis:
        diff: compara a saída sequencial com a saída concorrente
*/

typedef struct
{
    int row;
    int col;
    float **matrix;
} t_matrix;

t_matrix *A;
t_matrix *B;
t_matrix *C;
t_matrix *sequential_matrix;
FILE *file;

int n_threads;

void *multiply_concurrent_matrix(void *arg)
{
    int thread_id = *(int *)arg;
    for (int i = thread_id; i < A->row; i += n_threads)
    {
        for (int j = 0; j < B->col; j++)
        {
            for (int k = 0; k < A->row; k++)
            {
                C->matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

void print_matrix(char *filename, t_matrix *matrix)
{
    file = fopen(filename, "w");
    for (int i = 0; i < matrix->row; i++)
    {
        for (int j = 0; j < matrix->col; j++)
        {
            fprintf(file, "%.6f ", matrix->matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void multiply_sequential_matrix()
{
    for (int i = 0; i < A->row; i++)
    {
        for (int j = 0; j < B->col; j++)
        {
            for (int k = 0; k < A->row; k++)
            {
                sequential_matrix->matrix[i][j] +=
                    A->matrix[i][k] * B->matrix[k][j];
            }
        }
    }
}

void fill_matrix(t_matrix *matrix, int fill_with_zeros)
{
    matrix->matrix = (float **)malloc(sizeof(float *) * matrix->row);
    if (matrix->matrix == NULL)
    {
        printf("Falha ao alocar memória.\n");
        exit(1);
    }

    for (int i = 0; i < matrix->row; i++)
    {
        matrix->matrix[i] = (float *)malloc(sizeof(float) * matrix->col);
        if (matrix->matrix[i] == NULL)
        {
            printf("Falha ao alocar memória.\n");
            exit(1);
        }
    }

    for (int i = 0; i < matrix->row; i++)
    {
        for (int j = 0; j < matrix->col; j++)
        {
            if (fill_with_zeros)
            {
                matrix->matrix[i][j] = 0;
            }
            else
            {
                fscanf(file, "%f", &matrix->matrix[i][j]);
            }
        }
    }
}

void init_matrix(char *filename, t_matrix *matrix)
{
    if (filename != NULL)
    {
        file = fopen(filename, "r");
        fscanf(file, "%d %d", &matrix->row, &matrix->col);
        fill_matrix(matrix, false);
        fclose(file);
        return;
    }

    matrix->row = A->row;
    matrix->col = B->col;
    fill_matrix(matrix, true);
    return;
}

/*
    Função para alocar memória para as matrizes
    A (arquivo 1), B (arquivo 2) e C (resultado)
*/
void allocate_mem_matrix()
{
    sequential_matrix = (t_matrix *)malloc(sizeof(t_matrix));
    if (sequential_matrix == NULL)
    {
        printf("Erro: não foi possível alocar memória para a matriz sequencial.\n");
        exit(-1);
    }

    A = (t_matrix *)malloc(sizeof(t_matrix));

    if (A == NULL)
    {
        printf("Erro: não foi possível alocar memória para a matriz A.\n");
        exit(-1);
    }

    B = (t_matrix *)malloc(sizeof(t_matrix));

    if (B == NULL)
    {
        printf("Erro: não foi possível alocar memória para a matriz B.\n");
        exit(-1);
    }

    C = (t_matrix *)malloc(sizeof(t_matrix));

    if (C == NULL)
    {
        printf("Erro: não foi possível alocar memória para a matriz C.\n");
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Use: <nome_arquivo_entrada1> <nome_arquivo_entrada2> <nome_arquivo_saida> <quantidade_threads>\n");
        exit(1);
    }

    pthread_t *threads;
    double start, end, elapsed_seq, elapsed_conc;

    n_threads = atoi(argv[4]);

    threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
    if (threads == NULL)
    {
        printf("Erro: não foi possível alocar memória para as threads.\n");
        exit(-1);
    }

    allocate_mem_matrix();

    init_matrix(argv[1], A);
    init_matrix(argv[2], B);
    init_matrix(NULL, C);

    GET_TIME(start);
    int *thread_ids = (int *)malloc(sizeof(int) * n_threads);
    if (thread_ids == NULL)
    {
        printf("Erro: não foi possível alocar memória para os ids das threads.\n");
        exit(-1);
    }

    for (int i = 0; i < n_threads; i++)
    {
        thread_ids[i] = i;
        pthread_create(threads + i, NULL, multiply_concurrent_matrix, &thread_ids[i]);
    }

    for (int i = 0; i < n_threads; i++)
        pthread_join(*(threads + i), NULL);

    print_matrix(argv[3], C);
    GET_TIME(end);

    elapsed_conc = end - start;

#ifdef INFO
    printf("Tempo de execução do cálculo de matrizes com %d thread(s): %.6f\n", n_threads, elapsed_conc);
#endif

    if (argv[5] != NULL && strcmp(argv[5], "diff") == 0)
    {
        start = 0;
        end = 0;
        GET_TIME(start);
        init_matrix(NULL, sequential_matrix);
        multiply_sequential_matrix();
        GET_TIME(end);
        elapsed_seq = end - start;
        print_matrix("output_sequential", sequential_matrix);

#ifdef INFO
        printf("Tempo de execução do cálculo de matrizes sequencial: %.6f\n", elapsed_seq);
#endif
        system("python3 ./diff.py output output_sequential");
    }

    file = fopen("result.csv", "a+");
    if (file == NULL)
    {
        printf("Erro: não foi possível abrir o arquivo de resultados.\n");
        exit(-1);
    }
    fseek(file, 0, SEEK_END);
    if (ftell(file) == 0)
    {
        fprintf(file, "tempo_seq, tempo_conc, n_threads, n_linhas, n_colunas\n");
    }

    fprintf(file, "%.6f, %.6f, %d, %d, %d\n", elapsed_seq, elapsed_conc, n_threads, A->row, A->col);

    free(threads);
    free(A);
    free(B);
    free(sequential_matrix);

    return 0;
}