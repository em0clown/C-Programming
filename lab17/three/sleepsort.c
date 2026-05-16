#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* sleep_element(void* arg) {
    int val = *(int*)arg;
    usleep(val * 100000); 
    printf("%d ", val);
    fflush(stdout);
    return NULL;
}

int main() {
    int arr[] = {12, 5, 2, 8, 1, 7, 4};
    int n = sizeof(arr) / sizeof(arr[0]);
    pthread_t threads[n];

    printf("Sleepsort запускается для массива: ");
    for(int i=0; i<n; i++) printf("%d ", arr[i]);
    printf("\nРезультат: ");

    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, sleep_element, &arr[i]);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("\nСортировка окончена.\n");
    return 0;
}