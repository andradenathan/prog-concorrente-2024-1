#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define PROGRAM_ARGS_SIZE 4
#define PRODUCERS 0
#define CONSUMERS 0
#define N 1000000

FILE *file;
sem_t full_slot, empty_slot;
sem_t mutex_producer, mutex_consumer;
int M;
int *buffer;

typedef struct product_consumers_t
{
    int thread_id;
    int is_prime;
} product_consumers_t;

int is_prime(int number) {}

void *producer(void *args)
{
    char *filename = (char *)args;
    file = fopen(filename, "rb");

    int integer;
    static int in = 0;
    for (int i = 0; i < N; i++)
    {
        sem_wait(&empty_slot);
        sem_wait(&mutex_producer);

        fread(&integer, sizeof(int), 1, file);

        buffer[in] = integer;
        in = (in + 1) % M;

        sem_post(&mutex_producer);
        sem_post(&full_slot);
    }

    fclose(file);
    pthread_exit(NULL);
}

void *consumer(void *args)
{
    product_consumers_t *thread_args = (product_consumers_t *)args;
    int integer;
    static int out = 0;

    for (int i = 0; i < N; i++)
    {
        sem_wait(&full_slot);
        sem_wait(&mutex_consumer);

        integer = buffer[out];
        out = (out + 1) % M;

        sem_post(&mutex_consumer);
        sem_post(&empty_slot);

        if (!is_prime(integer))
            continue;

        thread_args->is_prime++;
    }

    pthread_exit((void *)thread_args);
}

// Função helper para verificar se um ponteiro foi alocado corretamente.
void check_memory_allocation(void *pointer, char *variable_name)
{
    if (!pointer)
    {
        fprintf(stderr, "Erro de alocação de memória da variável %s\n", variable_name);
        exit(-1);
    }
}

/* Função helper para conferir se o arquivo binário foi lido ou escrito
    corretamente dado seu tamanho e o tamanho esperado.
*/
void check_file_size(size_t file_size, long unsigned int expected_size, char *filename)
{
    if (file_size < expected_size)
    {
        fprintf(stderr, "Erro ao ler/escrever no arquivo %s\n", filename);
        exit(-2);
    }
}

// Função para gerar os números aleatórios e inserir no arquivo binário.
void write_bin_integers(char *filename)
{
    int *integers;
    size_t file_size;

    integers = (int *)malloc(sizeof(int) * N);
    check_memory_allocation(integers, "integers");

    srand(time(NULL));
    for (int index = 0; index < N; index++)
    {
        integers[index] = rand() % N;
    }

    file = fopen(filename, "wb");
    check_memory_allocation(file, "file");

    file_size = fwrite(integers, sizeof(int), N, file);
    check_file_size(file_size, N, filename);

    fclose(file);
    free(integers);
}

// Função para ler os números inteiros do arquivo binário.
void read_bin_integers(int *numbers, char *filename)
{
    size_t file_size;
    file = fopen(filename, "rb");
    file_size = fread(numbers, sizeof(int), N, file);
    check_file_size(file_size, N, filename);
}

int main(int argc, char *argv[])
{
    pthread_t *threads;
    int n_threads_consumers;

    if (argc < PROGRAM_ARGS_SIZE)
    {
        fprintf(stderr, "Digite: %s <quantidade de threads consumidoras> "
                        "<tamanho M do buffer> <nome do arquivo de entrada>\n",
                argv[0]);
        return 1;
    }

    n_threads_consumers = atoi(argv[1]);

    M = atoi(argv[2]);

    buffer = malloc(sizeof(int) * M);
    check_memory_allocation(buffer, "buffer");

    char *filename = argv[3];
    write_bin_integers(filename);

    sem_init(&mutex_consumer, 0, 1);
    sem_init(&mutex_producer, 0, 1);
    sem_init(&full_slot, 0, 0);
    sem_init(&empty_slot, 0, M);

    threads = malloc(sizeof(pthread_t) * (PRODUCERS + CONSUMERS));
    check_memory_allocation(threads, "threads");

    if (pthread_create(&threads[0], NULL, producer, (void *)filename))
    {
        fprintf(stderr, "Erro ao criar a thread produtora\n");
        return 2;
    }

    for (int index = 0; index < n_threads_consumers; index++)
    {
        product_consumers_t *args = malloc(sizeof(product_consumers_t));
        args->thread_id = index;
        args->is_prime = 0;
        if (pthread_create(&threads[index + 1], NULL, consumer, (void *)args))
        {
            fprintf(stderr, "Erro ao criar a thread consumidora\n");
            return 3;
        }
    }

    sem_destroy(&mutex_consumer);
    sem_destroy(&mutex_producer);
    sem_destroy(&full_slot);
    sem_destroy(&empty_slot);
    free(buffer);
    free(threads);
    return 0;
}