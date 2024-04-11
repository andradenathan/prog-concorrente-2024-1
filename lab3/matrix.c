#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "./timer.h"

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

void multiply_concurrent_matrix()
{
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
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Use: <nome_arquivo_entrada1> <nome_arquivo_entrada2> <nome_arquivo_saida> <quantidade_threads>\n");
        exit(1);
    }

    pthread_t *threads;

    int rows_per_threads;
    int n_threads = atoi(argv[4]);

    threads = (pthread_t *)malloc(n_threads * sizeof(pthread_t));
    if (threads == NULL)
    {
        printf("Erro: não foi possível alocar memória para as threads.\n");
        exit(-1);
    }

    allocate_mem_matrix();

    init_matrix(argv[1], A);
    init_matrix(argv[2], B);
    // rows_per_threads = A->row / n_threads;

    printf("A: %d %d\n", A->row, A->col);
    printf("B: %d %d\n", B->row, B->col);
    printf("Sequential: %d %d\n", sequential_matrix->row, sequential_matrix->col);

    // TODO: Implementar a multiplicação de matrizes paralela

    printf("oi\n");

    free(threads);
    free(A);
    free(B);
    free(sequential_matrix);

    return 0;
}