#include <stdio.h>
#include <pthread.h>

int prime(long long int N)
{
    if (N < 2)
        return 0;

    if (N == 2)
        return 1;

    if (N % 2 == 0)
        return 0;

    for (int i = 3; i < sqrt(N) + 1; i += 2)
        if (N % i == 0)
            return 0;
    return 1;
}

int main(int argv, char **argc)
{
    if (argv != 2)
    {
        printf("Use: %s <numero_threads> <N>\n", argc[0]);
        return 1;
    }

    int N = atoll(argc[2]);
    int n_threads = atoi(argc[1]);

    return 0;
}