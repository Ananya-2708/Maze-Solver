#include "queue.h"
#include <cstdio>
#include <cstdlib>

void initQueue(Queue* q) {
    q->front = 0;
    q->rear  = 0;
    q->size  = 0;
}

bool isQueueEmpty(const Queue* q) {
    return q->size == 0;
}

bool isQueueFull(const Queue* q) {
    return q->size >= (MAX_SIZE * MAX_SIZE);
}

void enqueue(Queue* q, Position value) {
    if (isQueueFull(q)) {
        std::fprintf(stderr, "Queue overflow!\n");
        std::exit(EXIT_FAILURE);
    }
    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % (MAX_SIZE * MAX_SIZE);
    q->size++;
}

Position dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        std::fprintf(stderr, "Queue underflow!\n");
        std::exit(EXIT_FAILURE);
    }
    Position val = q->data[q->front];
    q->front = (q->front + 1) % (MAX_SIZE * MAX_SIZE);
    q->size--;
    return val;
}