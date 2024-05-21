#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARR_SIZE 100
#define N_THREADS 10

typedef struct
{
    int start;
    int end;
} thread_args_t;

int array[ARR_SIZE];
int thread_array[ARR_SIZE];

void *square(void *args)
{
    thread_args_t *thread_args = (thread_args_t *)args;
    for (int i = thread_args->start; i < thread_args->end; i++)
    {
        thread_array[i] *= thread_array[i];
    }
    pthread_exit(NULL);
}

void print_array()
{
    for (int i = 0; i < ARR_SIZE; i++)
    {
        printf("index: %d \t| %d * %d -> %d\n", i + 1, array[i], array[i], thread_array[i]);
    }
}

void init_array_from_file()
{
    FILE *file = fopen("input", "r");

    if (file == NULL)
    {
        printf("File not found\n");
        exit(-1);
    }

    int counter = 0;

    while (counter < ARR_SIZE && fscanf(file, "%d", &array[counter]) == 1)
    {
        counter++;
    }

    fclose(file);
}

void check_result()
{
    for (int i = 0; i < ARR_SIZE; i++)
    {
        if (array[i] * array[i] != thread_array[i])
        {
            printf("Error at index %d. The thread may have calculated it incorrectly.\n", i);
            return;
        }
    }
}

void copy_array_to_thread_array()
{
    for (int i = 0; i < ARR_SIZE; i++)
    {
        thread_array[i] = array[i];
    }
}

int main(void)
{
    pthread_t thread_id[N_THREADS];
    thread_args_t thread_args[N_THREADS];

    init_array_from_file();
    copy_array_to_thread_array();

    int block_size = ARR_SIZE / N_THREADS;

    for (int i = 0; i < N_THREADS; i++)
    {
        thread_args[i].start = i * block_size;
        thread_args[i].end = (i + 1) * block_size;
        if (pthread_create(&thread_id[i], NULL, square, (void *)&thread_args[i]))
            printf("Error creating thread\n");
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(thread_id[i], NULL);
    }

    print_array();

    check_result();

    pthread_exit(NULL);

    return 0;
}