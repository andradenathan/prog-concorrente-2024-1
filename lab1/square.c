#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARR_SIZE 10
#define N_THREADS 10

int array[ARR_SIZE];
int thread_array[ARR_SIZE];

void *square(void *arg)
{
    int *index = (int *)arg;
    thread_array[*index] *= thread_array[*index];
    pthread_exit(NULL);
}

void print_array()
{
    for (int i = 0; i < ARR_SIZE; i++)
    {
        printf("index: %d | %d * %d -> %d\n", i, array[i], array[i], thread_array[i]);
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
    int thread_args[N_THREADS];

    init_array_from_file();
    copy_array_to_thread_array();

    for (int i = 1; i <= N_THREADS; i++)
    {
        thread_args[i] = i;
        if (pthread_create(&thread_id[i - 1], NULL, square, (void *)&thread_args[i - 1]))
            printf("Error creating thread\n");
    }

    for (int i = 0; i < N_THREADS; i++)
    {
        pthread_join(thread_id[i], NULL);
    }

    print_array();

    pthread_exit(NULL);

    return 0;
}