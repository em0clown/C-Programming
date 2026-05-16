#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void cleanup_handler(void *arg) {
    printf("[Cleanup] Поток %ld завершен через cancel и очищен\n", (long)arg);
}

void* thread_work(void* arg) {
    int id = *(int*)arg;
    pthread_cleanup_push(cleanup_handler, (void*)(long)id);

    for (int i = 1; i <= 5; i++) {
        printf("Дочерний поток %d: строка %d\n", id, i);
        sleep(1);
    }

    pthread_cleanup_pop(1);
    return NULL;
}

int main() {
    pthread_t threads[4];
    int ids[4];

    printf("--- Запуск 4-х потоков (Упр 1-4) ---\n");
    for (int i = 0; i < 4; i++) {
        ids[i] = i + 1;
        pthread_create(&threads[i], NULL, thread_work, &ids[i]);
    }

    sleep(2);
    printf("Main: Прошло 2 секунды, отменяю потоки...\n");
    for (int i = 0; i < 4; i++) {
        pthread_cancel(threads[i]);
    }

    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Main: Все потоки завершены. Родитель выходит.\n");
    return 0;
}