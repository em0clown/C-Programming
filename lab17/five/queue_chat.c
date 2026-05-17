#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define QUEUE_SIZE 10
#define MSG_LEN 128

typedef struct {
    char messages[QUEUE_SIZE][MSG_LEN];
    int count;
    int head;
    int tail;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} msg_queue_t;

msg_queue_t q = {
    .count = 0, 
    .head = 0, 
    .tail = 0, 
    .lock = PTHREAD_MUTEX_INITIALIZER, 
    .not_full = PTHREAD_COND_INITIALIZER, 
    .not_empty = PTHREAD_COND_INITIALIZER
};

int msgSend(char* msg) {
    pthread_mutex_lock(&q.lock);
    
    while (q.count == QUEUE_SIZE) {
        pthread_cond_wait(&q.not_full, &q.lock);
    }
    
    strncpy(q.messages[q.tail], msg, MSG_LEN - 1);
    q.messages[q.tail][MSG_LEN - 1] = '\0';
    
    q.tail = (q.tail + 1) % QUEUE_SIZE;
    q.count++;
    
    pthread_cond_signal(&q.not_empty);
    pthread_mutex_unlock(&q.lock);
    
    return strlen(msg);
}

int msgRecv(char* buf) {
    pthread_mutex_lock(&q.lock);
    
    while (q.count == 0) {
        pthread_cond_wait(&q.not_empty, &q.lock);
    }
    
    strncpy(buf, q.messages[q.head], MSG_LEN - 1);
    q.messages[q.head][MSG_LEN - 1] = '\0';
    
    q.head = (q.head + 1) % QUEUE_SIZE;
    q.count--;
    
    pthread_cond_signal(&q.not_full);
    pthread_mutex_unlock(&q.lock);
    
    return strlen(buf);
}

void* client(void* arg) {
    char* name = (char*)arg;
    
    char* phrases1[] = {
        "Vsem privet, ya podnyal docker kontejner",
        "Pochemu nginx vydaet 502 bad gateway?",
        "Sejchas perezagruzhu servis, podozhdite minutu",
        "Logi chistye, vrode vse zarabotalo",
        "Zakanchivayu rabotu, vsem poka"
    };
    
    char* phrases2[] = {
        "Privet! Sejchas proveru bazu dannyh",
        "U menya vse ok, vizhu tvoi zaprosy",
        "Dobavil novyj API metod dlya video",
        "Nuzhno dobavit' pamyati dlya kontejнера",
        "Prinyato, tozhe skoro vyhodu"
    };

    char** my_phrases = (strcmp(name, "Klient_1") == 0) ? phrases1 : phrases2;
    int n = 5;
    int indices[] = {0, 1, 2, 3, 4};

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    char msg[MSG_LEN];
    for(int i = 0; i < n; i++) {
        snprintf(msg, MSG_LEN, "[%s] %s", name, my_phrases[indices[i]]);
        msgSend(msg);
        
        usleep((rand() % 1000 + 500) * 1000); 
    }
    return NULL;
}

void* server(void* arg) {
    char buf[MSG_LEN];
    while(1) {
        msgRecv(buf);
        printf("[Server] Polucheno: %s\n", buf);
        usleep(400000); 
    }
}

#ifndef CHECK_TESTING
int main() {
    srand(time(NULL));
    
    pthread_t tid_server, tid_c1, tid_c2;

    printf("=== Zapusk odnostoronnego chata ===\n");

    pthread_create(&tid_server, NULL, server, NULL);
    pthread_detach(tid_server);
    pthread_create(&tid_c1, NULL, client, "Klient_1");
    pthread_create(&tid_c2, NULL, client, "Klient_2");
    pthread_join(tid_c1, NULL);
    pthread_join(tid_c2, NULL);

    printf("\n=== Klienty otpravili vse soobsheniya ===\n");
    sleep(2); 
    
    printf("Zavershenie raboty.\n");
    return 0;
}
#endif