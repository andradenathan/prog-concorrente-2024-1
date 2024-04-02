#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

float *arr;

typedef struct
{
    int thread_id;
    int block_size;
} thread_args_t;

void *sum(void *args)
{
    thread_args_t *thread_args = (thread_args_t *)args;

    float *local_sum;
    local_sum = (float *)malloc(sizeof(float));

    if (local_sum == NULL)
    {
        printf("Erro: não foi possível alocar memória para a soma local.\n");
        exit(-1);
    }
    long int start = thread_args->thread_id * thread_args->block_size;
    long int end = start + thread_args->block_size;

    printf("thread_id: %d | start: %ld | end: %ld\n",
           thread_args->thread_id,
           start,
           end);

    *local_sum = 0;

    for (long int i = start; i < end; i++)
    {
        *local_sum += arr[i];
    }

    pthread_exit((void *)local_sum);
}

void fill_array(long int n)
{
    arr = (float *)malloc(n * sizeof(float));

    if (arr == NULL)
    {
        printf("Erro: não foi possível alocar memória para o vetor.\n");
        exit(-1);
    }

    for (long int i = 0; i < n; i++)
    {
        scanf("%f", &arr[i]);
    }
}

int main(int argc, char *argv[])
{
    long int n;
    int n_threads;
    long int block_size;

    pthread_t *tid;

    float result = 0;

    float *concurrent_result;
    float sequential_result = 0;

    if (argc < 2)
    {
        printf("Use: %s <numero de threads> \n", argv[0]);
        return 1;
    }

    scanf("%ld", &n);

    fill_array(n);

    n_threads = atoi(argv[1]);

    tid = (pthread_t *)malloc(sizeof(pthread_t) * n_threads);

    if (tid == NULL)
    {
        printf("Erro: não foi possível alocar memória para as threads.\n");
        return 2;
    }

    block_size = n / n_threads;

    if (!block_size)
        printf("Aviso: a quantidade de threads é maior que o tamanho do vetor, isto implica que o programa será sequencial.\n");

    for (int i = 0; i < n; i++)
        sequential_result += arr[i];

    for (int i = 0; i < n_threads; i++)
    {
        thread_args_t *args = (thread_args_t *)malloc(sizeof(thread_args_t));
        if (args == NULL)
        {
            printf("Erro: não foi possível alocar memória para os argumentos da thread %d.\n", i);
            return 3;
        }
        args->thread_id = i;
        args->block_size = block_size;

        if (pthread_create(tid + i, NULL, sum, (void *)args))
        {
            printf("Erro: não foi possível criar a thread %d.\n", i);
            return 4;
        }
    }

    for (int i = 0; i < n_threads; i++)
    {
        if (pthread_join(*(tid + i), (void **)&concurrent_result))
        {
            printf("Erro: não foi possível buscar o resultado da thread %d.\n", i);
            return 5;
        }

        result += *concurrent_result;
    }

    if (n % n_threads)
    {
        printf("Processando os dados restantes...\n");
        for (int i = n - (n % n_threads); i < n; i++)
            result += arr[i];
    }

    printf("Resultado da soma sequencial: %f\n", sequential_result);
    printf("Resultado da soma concorrente: %f\n", result);

    free(arr);
    free(tid);
    free(concurrent_result);

    return 0;
}