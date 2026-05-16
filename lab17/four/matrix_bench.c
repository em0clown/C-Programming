#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int N, num_threads;
int **A, **B, **C;

typedef struct { int start_row, end_row; } ThreadData;

void* multiply(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) C[i][j] += A[i][k] * B[k][j];
        }
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    N = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    A = malloc(N * sizeof(int*)); B = malloc(N * sizeof(int*)); C = malloc(N * sizeof(int*));
    for(int i=0; i<N; i++) {
        A[i] = malloc(N * sizeof(int)); B[i] = malloc(N * sizeof(int)); C[i] = malloc(N * sizeof(int));
        for(int j=0; j<N; j++) { A[i][j] = 1; B[i][j] = 1; }
    }

    pthread_t threads[num_threads];
    ThreadData t_data[num_threads];
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    int rows_per_thread = N / num_threads;
    for (int i = 0; i < num_threads; i++) {
        t_data[i].start_row = i * rows_per_thread;
        t_data[i].end_row = (i == num_threads - 1) ? N : (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, multiply, &t_data[i]);
    }
    for (int i = 0; i < num_threads; i++) pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    
    printf("%d,%d,%.2f\n", num_threads, N, time_ms);

    return 0;
}