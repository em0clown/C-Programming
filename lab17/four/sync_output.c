#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    int row_start, row_end, N;
    int **A, **B, **C;
} Task;

void* multiply(void* arg) {
    Task* t = (Task*)arg;
    for (int i = t->row_start; i < t->row_end; i++) {
        for (int j = 0; j < t->N; j++) {
            t->C[i][j] = 0;
            for (int k = 0; k < t->N; k++) {
                t->C[i][j] += t->A[i][k] * t->B[k][j];
            }
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) return printf("Usage: %s <N> <threads>\n", argv[0]);
    int N = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    // Выделение памяти и заполнение единицами (Упр 8a)
    int **A = malloc(N * sizeof(int*));
    int **B = malloc(N * sizeof(int*));
    int **C = malloc(N * sizeof(int*));
    for(int i=0; i<N; i++) {
        A[i] = malloc(N * sizeof(int)); B[i] = malloc(N * sizeof(int)); C[i] = malloc(N * sizeof(int));
        for(int j=0; j<N; j++) { A[i][j] = 1; B[i][j] = 1; }
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t threads[num_threads];
    Task tasks[num_threads];
    int rows_per_thread = N / num_threads;

    for (int i = 0; i < num_threads; i++) {
        tasks[i] = (Task){i * rows_per_thread, (i == num_threads - 1) ? N : (i + 1) * rows_per_thread, N, A, B, C};
        pthread_create(&threads[i], NULL, multiply, &tasks[i]);
    }

    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    printf("N=%d, Threads=%d, Time: %.2f ms\n", N, num_threads, time_ms);

    return 0;
}