#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct {
    int *items;
    int capacity;
    int size;
    int head;
    int tail;
} Queue;

int queue_init(Queue *queue, int capacity);
void queue_destroy(Queue *queue);
bool queue_is_empty(const Queue *queue);
bool queue_is_full(const Queue *queue);
int queue_push(Queue *queue, int value);
int queue_pop(Queue *queue, int *value);

#endif
