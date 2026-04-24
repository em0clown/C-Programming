#include <stdio.h>
#include <stdlib.h>

// Скомпилируй и запусти меня

int* init(int *arr, int n) { 
    arr = malloc(n * sizeof(int));
    for (long i = 0; i < n; ++i) { 
        arr[i] = i;    
    }
    return arr;
}

int main() {
    int *arr = NULL;
    int n = 10;

    arr = init(arr, n);
    for (int i = 0; i < n; ++i) {
        printf("%d\n", arr[i]);
    }
    free(arr);
    return 0;
}