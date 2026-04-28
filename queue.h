#ifndef QUEUE_H
#define QUEUE_H

#include "maze.h"

// Simple circular queue for Position (FIFO)
struct Queue {
    Position data[MAX_SIZE * MAX_SIZE];
    int front;
    int rear;
    int size;
};

void initQueue(Queue* q);
bool isQueueEmpty(const Queue* q);
bool isQueueFull(const Queue* q);
void enqueue(Queue* q, Position value);
Position dequeue(Queue* q);

#endif