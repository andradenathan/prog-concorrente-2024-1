#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define MAX_THREADS 100 // Máximo de threads suportadas
#define TRUE 1
#define FALSE 0

struct ThreadData
{
    long long int N;
    long long int *primes;
    pthread_mutex_t *mutex;
    long long int next_task;
};

int is_prime(long long int N)
{
    if (N < 2)
        return FALSE;

    if (N == 2)
        return TRUE;

    if (N % 2 == 0)
        return FALSE;

    for (int i = 3; i < sqrt(N) + 1; i += 2)
        if (N % i == 0)
            return FALSE;
    return TRUE;
}

void *worker(void *arg)
{
    struct ThreadData *data = (struct ThreadData *)arg;
    long long int task;
    while (1)
    {
        pthread_mutex_lock(data->mutex);
        task = data->next_task;
        data->next_task++;
        pthread_mutex_unlock(data->mutex);

        if (task > data->N)
            break;

        if (is_prime(task))
        {
            pthread_mutex_lock(data->mutex);
            *(data->primes + task) = TRUE;
            pthread_mutex_unlock(data->mutex);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <N> <num_threads>\n", argv[0]);
        return 1;
    }

    long long int N = atoll(argv[1]);
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0 || num_threads > MAX_THREADS)
    {
        printf("Invalid number of threads. Please provide a number between 1 and %d.\n", MAX_THREADS);
        return 1;
    }

    double start, end = 0;

    pthread_t threads[num_threads];
    struct ThreadData thread_data[num_threads];
    long long int primes[N + 1]; // Armazena os resultados de primos
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    GET_TIME(start);
    // Inicializa o array de primos com FALSE
    for (long long int i = 0; i <= N; i++)
    {
        primes[i] = FALSE;
    }

    // Inicializa dados das threads
    for (int i = 0; i < num_threads; i++)
    {
        thread_data[i].N = N;
        thread_data[i].primes = primes;
        thread_data[i].mutex = &mutex;
        thread_data[i].next_task = 2; // Começa com o primeiro número a ser testado
    }

    // Cria as threads
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, worker, (void *)&thread_data[i]);
    }

    // Aguarda todas as threads terminarem
    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Imprime os números primos encontrados
    printf("Prime numbers up to %lld:\n", N);
    for (long long int i = 2; i <= N; i++)
    {
        if (primes[i])
        {
            printf("%lld ", i);
        }
    }
    GET_TIME(end);
    printf("\n");
    printf("Tempo de execução do algoritmo: %lf com N = %lld e %d thread(s)\n", end - start, N, num_threads);
    return 0;
}
