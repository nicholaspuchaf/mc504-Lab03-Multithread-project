#include "queue.h"

#include <stdlib.h>

int queue_init(Queue *queue, int capacity)
{
    if (queue == NULL || capacity <= 0) {
        return -1;
    }

    queue->items = calloc((size_t)capacity, sizeof(int));
    if (queue->items == NULL) {
        return -1;
    }

    queue->capacity = capacity;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;

    return 0;
}

void queue_destroy(Queue *queue)
{
    if (queue == NULL) {
        return;
    }

    free(queue->items);
    queue->items = NULL;
    queue->capacity = 0;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
}

bool queue_is_empty(const Queue *queue)
{
    return queue == NULL || queue->size == 0;
}

bool queue_is_full(const Queue *queue)
{
    return queue != NULL && queue->size == queue->capacity;
}

int queue_push(Queue *queue, int value)
{
    if (queue == NULL || queue_is_full(queue)) {
        return -1;
    }

    queue->items[queue->tail] = value;
    queue->tail = (queue->tail + 1) % queue->capacity;
    queue->size++;

    return 0;
}

int queue_pop(Queue *queue, int *value)
{
    if (queue == NULL || value == NULL || queue_is_empty(queue)) {
        return -1;
    }

    *value = queue->items[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    queue->size--;

    return 0;
}
