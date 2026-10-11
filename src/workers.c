#include "queue.h"
#include "utils.h"
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t queue_items;
pthread_mutex_t queue_lock;

void *worker_thread(void *arg) {
    while (1) {
        sem_wait(&queue_items);
        // lock current to thread to get client fd from queue
        pthread_mutex_lock(&queue_lock);
        int client_fd = queue_pop();
        pthread_mutex_unlock(&queue_lock);
        if (client_fd >= 0) {
            handle_client(client_fd);
            close(client_fd);
        }
    }
    return NULL;
}

void init_workers(int worker_count) {
    sem_init(&queue_items, 0, 0);
    pthread_attr_t t_attr;
    pthread_attr_init(&t_attr);
    pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&queue_lock, NULL);
    for (int i = 0; i < worker_count; i++) {
        pthread_t tid;
        pthread_create(&tid, &t_attr, worker_thread, NULL);
    }
}

void dispatch_client(int client_fd) {
    pthread_mutex_lock(&queue_lock);
    queue_push(client_fd);
    pthread_mutex_unlock(&queue_lock);
    sem_post(&queue_items);
}
