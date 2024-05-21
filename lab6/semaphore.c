#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define PRODUCERS 0
#define CONSUMERS 0
#define N 1000000

FILE *file;
sem_t full_slot, empty_slot;
sem_t mutex_producer, mutex_consumer;
int M;
int *buffer;

void *is_prime_number(void *args) {}

void *producer(void *args) {}

void *consumer(void *args) {}

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

    if (argc < 4)
    {
        fprintf(stderr, "Digite: %s <quantidade de threads consumidoras> "
                        "<tamanho M do buffer> <nome do arquivo de entrada>\n",
                argv[0]);
        return 1;
    }

    int *int_sequence = malloc(sizeof(int) * N);
    check_memory_allocation(int_sequence, "int_sequence");

    M = atoi(argv[2]);

    buffer = malloc(sizeof(int) * M);
    check_memory_allocation(buffer, "buffer");

    char *filename = argv[3];

    write_bin_integers(filename);

    read_bin_integers(int_sequence, filename);

    threads = malloc(sizeof(pthread_t) * (PRODUCERS + CONSUMERS));
    check_memory_allocation(threads, "threads");

    sem_init(&mutex_consumer, 0, 1);
    sem_init(&mutex_producer, 0, 1);
    sem_init(&full_slot, 0, 0);
    sem_init(&empty_slot, 0, M);

    // TODO: Implementar a parte do produtor e consumidor.

    free(int_sequence);

    return 0;
}