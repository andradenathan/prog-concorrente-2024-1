#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define PROGRAM_ARGS_SIZE 4
#define N 100000

FILE *file;
sem_t full_slot, empty_slot; // semáforo para representar buffer vazio e buffer cheio.
sem_t mutex;                 // semáforo para realizar mutex (geral).
int M;
int *buffer;
int primes; // número de primos total no arquivo calculado ao inserir os elementos.

typedef struct consumers_t
{
    int thread_id;
    int is_prime;
} consumers_t;

// Função que checa a primalidade de um determinado número.
int is_prime(long long int number)
{
    int i;
    if (number < 1)
        return 0;

    if (number == 2)
        return 1;

    if (number % 2 == 0)
        return 0;

    for (i = 3; i < sqrt(number) + 1; i += 2)
        if (number % i == 0)
            return 0;

    return 1;
}

// Função para inserir um determinado número inteiro no buffer.
void insert(long long int integer)
{
    static int in = 0;
    sem_wait(&empty_slot); // decrementa o semáforo de slots vazios
    sem_wait(&mutex);      // realiza exclusão mútua
    buffer[in] = integer;
    in = (in + 1) % M;
    sem_post(&mutex);     // finaliza a exclusão mútua
    sem_post(&full_slot); // libera a thread consumidora
}

long long int remove_integer()
{
    int integer;
    static int out = 0;
    sem_wait(&full_slot); // aguarda o produtor inserir um valor
    sem_wait(&mutex);     // realiza exclusão mútua
    integer = buffer[out];
    out = (out + 1) % M;
    sem_post(&mutex);      // finaliza a exclusão mútua
    sem_post(&empty_slot); // libera a thread produtora
    return integer;
}

void *producer(void *args)
{
    char *filename = (char *)args;
    file = fopen(filename, "rb");

    int integer;

    // insere elementos no buffer enquanto tiver elementos no arquivo
    while (fread(&integer, sizeof(int), 1, file))
        insert(integer);

    fclose(file);

    // insere o valor de -1 para que as threads consumidoras saiam do loop
    for (int i = 0; i < M; i++)
        insert(-1);

    pthread_exit(NULL);
}

void *consumer(void *args)
{
    consumers_t *thread_args = (consumers_t *)args;
    long long int integer;

    while (1)
    {
        integer = remove_integer();
        if (integer == -1)
        {
            // insere de volta o -1 para que outras threads consumidoras também possam sair
            insert(-1);
            break;
        }

        if (is_prime(integer))
        {
            thread_args->is_prime++;
        }
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
    long long int *integers;
    size_t file_size;

    integers = (long long int *)malloc(sizeof(long long int) * N);
    check_memory_allocation(integers, "integers");

    srand(time(NULL));
    for (long long int index = 0; index < N; index++)
    {
        integers[index] = rand() % N;
        if (is_prime(integers[index]))
        {
            primes++;
        }
    }

    file = fopen(filename, "wb");
    check_memory_allocation(file, "file");

    file_size = fwrite(integers, sizeof(long long int), N, file);
    check_file_size(file_size, N, filename);

    fclose(file);
    free(integers);
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

    // inicializa os semáforos
    sem_init(&mutex, 0, 1);      // semáforo binário de exclusão mútua
    sem_init(&full_slot, 0, 0);  // semáforo indicador de buffer cheio
    sem_init(&empty_slot, 0, M); // semáforo indicador de buffer vazio

    threads = malloc(sizeof(pthread_t) * (1 + n_threads_consumers));
    check_memory_allocation(threads, "threads");

    if (pthread_create(&threads[0], NULL, producer, (void *)filename))
    {
        fprintf(stderr, "Erro ao criar a thread produtora\n");
        return 2;
    }

    for (int index = 1; index <= n_threads_consumers; index++)
    {
        consumers_t *args = malloc(sizeof(consumers_t));
        args->thread_id = index;
        args->is_prime = 0;
        if (pthread_create(&threads[index], NULL, consumer, (void *)args))
        {
            fprintf(stderr, "Erro ao criar a thread consumidora\n");
            return 3;
        }
    }

    consumers_t *args = NULL;
    int thread_computed_primes = 0;

    for (int index = 1; index <= n_threads_consumers; index++)
    {
        if (pthread_join(threads[index], (void **)&args))
        {
            fprintf(stderr, "Erro ao esperar a thread consumidora\n");
            return 4;
        }

        thread_computed_primes += args->is_prime;

        printf(
            "Thread %d encontrou %d números primos\n",
            args->thread_id,
            args->is_prime);
    }

    printf("O arquivo %s contém %d números primos\n", filename, primes);
    printf("Número total de primos encontrados pelas threads: %d\n", thread_computed_primes);

    sem_destroy(&mutex);
    sem_destroy(&full_slot);
    sem_destroy(&empty_slot);
    free(args);
    free(buffer);
    free(threads);
    return 0;
}
