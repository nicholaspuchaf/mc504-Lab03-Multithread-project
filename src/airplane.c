#include "airplane.h"

#include <stdio.h>
#include <time.h>

static void sleep_ms(int milliseconds)
{
    struct timespec duration = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (long)(milliseconds % 1000) * 1000000L,
    };

    nanosleep(&duration, NULL);
}

static const char *request_label(RequestType request)
{
    return request == REQUEST_LANDING ? "pouso" : "decolagem";
}

void *airplane_thread(void *arg)
{
    AirplaneThreadArgs *args = arg;
    Airport *airport;
    Airplane *airplane;

    if (args == NULL || args->airport == NULL || args->airplane == NULL) {
        return NULL;
    }

    airport = args->airport;
    airplane = args->airplane;

    pthread_mutex_lock(&airport->mutex);

    if (airplane->request == REQUEST_LANDING && airplane->emergency) {
        if (queue_push(&airport->emergency_landing_queue, airplane->id) != 0) {
            airplane->state = PLANE_LEFT;
        }
    } else if (airplane->request == REQUEST_LANDING) {
        if (queue_push(&airport->landing_queue, airplane->id) != 0) {
            airplane->state = PLANE_LEFT;
        }
    } else {
        if (queue_push(&airport->takeoff_queue, airplane->id) != 0) {
            airplane->state = PLANE_LEFT;
        }
    }

    if (airplane->state == PLANE_LEFT) {
        airport->completed_planes++;
        if (airport->config.no_animation) {
            printf("Aviao A%02d deixou o aeroporto: fila cheia.\n", airplane->id);
        }
        pthread_cond_signal(&airport->tower_event);
        pthread_mutex_unlock(&airport->mutex);
        return NULL;
    }

    if (airport->config.no_animation) {
        printf("Aviao A%02d entrou na fila de %s%s.\n",
               airplane->id,
               request_label(airplane->request),
               airplane->emergency ? " em emergencia" : "");
    }

    pthread_cond_signal(&airport->tower_event);

    while (!airplane->cleared) {
        pthread_cond_wait(&airplane->authorized, &airport->mutex);
    }

    if (airport->config.no_animation) {
        printf("Aviao A%02d autorizado para %s na pista %d.\n",
               airplane->id,
               request_label(airplane->request),
               airplane->runway_assigned);
    }

    pthread_mutex_unlock(&airport->mutex);

    sleep_ms(airport->config.runway_time_ms);

    pthread_mutex_lock(&airport->mutex);

    if (airport_release_runway(airport, airplane->runway_assigned, airplane->id) == 0) {
        if (airplane->request == REQUEST_LANDING) {
            airport->landings_completed++;
            if (airplane->emergency) {
                airport->emergencies_completed++;
            }
        } else {
            airport->takeoffs_completed++;
        }

        airplane->state = PLANE_DONE;
        airport->completed_planes++;
        sem_post(&airport->available_runways);

        if (airport->config.no_animation) {
            printf("Aviao A%02d concluiu %s e liberou a pista %d.\n",
                   airplane->id,
                   request_label(airplane->request),
                   airplane->runway_assigned);
        }
    }

    pthread_cond_signal(&airport->tower_event);
    pthread_mutex_unlock(&airport->mutex);

    return NULL;
}
