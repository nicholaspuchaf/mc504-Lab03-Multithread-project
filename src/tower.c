#include "tower.h"

#include <errno.h>
#include <semaphore.h>
#include <stdio.h>

static int has_waiting_planes(const Airport *airport)
{
    return airport->emergency_landing_queue.size > 0 ||
           airport->landing_queue.size > 0 ||
           airport->takeoff_queue.size > 0;
}

static int choose_next_plane(Airport *airport, int *plane_id)
{
    if (!queue_is_empty(&airport->emergency_landing_queue)) {
        airport->config.landing_count_before_takeoff++;
        return queue_pop(&airport->emergency_landing_queue, plane_id);
    }

    if (!queue_is_empty(&airport->takeoff_queue) &&
        airport->config.landing_count_before_takeoff >= airport->config.max_consecutive_landings) {
        airport->config.landing_count_before_takeoff = 0;
        return queue_pop(&airport->takeoff_queue, plane_id);
    }

    if (!queue_is_empty(&airport->landing_queue)) {
        airport->config.landing_count_before_takeoff++;
        return queue_pop(&airport->landing_queue, plane_id);
    }

    if (!queue_is_empty(&airport->takeoff_queue)) {
        airport->config.landing_count_before_takeoff = 0;
        return queue_pop(&airport->takeoff_queue, plane_id);
    }

    return -1;
}

void *tower_thread(void *arg)
{
    TowerThreadArgs *args = arg;
    Airport *airport;
    Airplane *airplanes;

    if (args == NULL || args->airport == NULL || args->airplanes == NULL) {
        return NULL;
    }

    airport = args->airport;
    airplanes = args->airplanes;

    pthread_mutex_lock(&airport->mutex);

    while (airport->config.running || has_waiting_planes(airport)) {
        int plane_id = -1;
        int runway_id = -1;
        Airplane *airplane;

        while (airport->config.running && !has_waiting_planes(airport)) {
            pthread_cond_wait(&airport->tower_event, &airport->mutex);
        }

        if (!has_waiting_planes(airport)) {
            continue;
        }

        if (sem_trywait(&airport->available_runways) != 0) {
            if (errno == EAGAIN) {
                pthread_cond_wait(&airport->tower_event, &airport->mutex);
                continue;
            }

            fprintf(stderr, "Torre: erro ao consultar pistas disponiveis.\n");
            continue;
        }

        if (choose_next_plane(airport, &plane_id) != 0) {
            sem_post(&airport->available_runways);
            continue;
        }

        airplane = &airplanes[plane_id];
        runway_id = airport_assign_runway(airport, airplane->id, airplane->request);
        if (runway_id < 0) {
            sem_post(&airport->available_runways);
            continue;
        }

        airplane->runway_assigned = runway_id;
        airplane->cleared = true;
        airplane->state = airplane->request == REQUEST_LANDING ? PLANE_LANDING : PLANE_TAKING_OFF;

        if (airport->config.no_animation) {
            printf("Torre autorizou A%02d para %s na pista %d.\n",
                   airplane->id,
                   airplane->request == REQUEST_LANDING ? "pouso" : "decolagem",
                   runway_id);
        }

        pthread_cond_signal(&airplane->authorized);
    }

    pthread_mutex_unlock(&airport->mutex);

    return NULL;
}
