#include "airplane.h"

#include <stdio.h>

void *airplane_thread(void *arg)
{
    AirplaneThreadArgs *args = arg;

    if (args == NULL || args->airport == NULL || args->airplane == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&args->airport->mutex);
    printf("Aviao A%02d aguardando implementacao da simulacao.\n", args->airplane->id);
    pthread_mutex_unlock(&args->airport->mutex);

    return NULL;
}
