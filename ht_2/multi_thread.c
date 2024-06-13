#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 2

long long sum = 0;
pthread_mutex_t mutex;

void* sum_runner(void* arg) {
    long long start = *((long long*)arg);
    long long end = start + 50000; // Cada hilo suma 50000 números
    long long partial_sum = 0;

    for (long long i = start; i < end; i++) {
        partial_sum += i;
    }

    pthread_mutex_lock(&mutex);
    sum += partial_sum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

int main() {
    pthread_t threads[NUM_THREADS];
    long long args[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    for (long long i = 0; i < NUM_THREADS; i++) {
        args[i] = i * 50000 + 1; // Los rangos son 1-50000 y 50001-100000
        pthread_create(&threads[i], NULL, sum_runner, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Sum: %lld\n", sum);
    printf("Time taken: %f seconds\n", cpu_time_used);

    pthread_mutex_destroy(&mutex);

    return 0;
}
