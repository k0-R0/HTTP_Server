#include "queue.h"

#define QUEUE_CAPACITY 64
int queue[QUEUE_CAPACITY];
int front, size;

int queue_push(int val) {
    if (size == QUEUE_CAPACITY)
        return 1;
    queue[(front + size) % QUEUE_CAPACITY] = val;
    size++;
    return 0;
}

int queue_pop() {
    if (size == 0)
        return -1;
    int ret = queue[front];
    front = (front + 1) % QUEUE_CAPACITY;
    size--;
    return ret;
}
