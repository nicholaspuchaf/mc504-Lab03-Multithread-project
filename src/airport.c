#include "airport.h"

#include <stdlib.h>
#include <string.h>

Config airport_default_config(void)
{
    Config config = {
        .total_planes = 10,
        .runway_count = 2,
        .max_queue_size = 32,
        .max_consecutive_landings = 3,
        .landing_count_before_takeoff = 0,
        .runway_time_ms = 700,
        .arrival_delay_ms = 120,
        .landing_probability = 65,
        .emergency_probability = 20,
        .no_animation = false,
        .running = true,
    };

    return config;
}

int airport_init(Airport *airport, Config config)
{
    if (airport == NULL || config.runway_count <= 0 || config.max_queue_size <= 0) {
        return -1;
    }

    memset(airport, 0, sizeof(*airport));
    airport->config = config;
    airport->completed_planes = 0;
    airport->landings_completed = 0;
    airport->takeoffs_completed = 0;
    airport->emergencies_completed = 0;
    airport->runways = calloc((size_t)config.runway_count, sizeof(Runway));
    if (airport->runways == NULL) {
        return -1;
    }

    for (int i = 0; i < config.runway_count; i++) {
        airport->runways[i].id = i;
        airport->runways[i].occupied_by = -1;
        airport->runways[i].operation = REQUEST_LANDING;
    }

    if (queue_init(&airport->emergency_landing_queue, config.max_queue_size) != 0 ||
        queue_init(&airport->landing_queue, config.max_queue_size) != 0 ||
        queue_init(&airport->takeoff_queue, config.max_queue_size) != 0) {
        airport_destroy(airport);
        return -1;
    }
    airport->queues_initialized = true;

    if (pthread_mutex_init(&airport->mutex, NULL) != 0) {
        airport_destroy(airport);
        return -1;
    }
    airport->mutex_initialized = true;

    if (pthread_cond_init(&airport->tower_event, NULL) != 0) {
        airport_destroy(airport);
        return -1;
    }
    airport->tower_event_initialized = true;

    if (sem_init(&airport->available_runways, 0, (unsigned int)config.runway_count) != 0) {
        airport_destroy(airport);
        return -1;
    }
    airport->semaphore_initialized = true;

    return 0;
}

void airport_destroy(Airport *airport)
{
    if (airport == NULL) {
        return;
    }

    if (airport->semaphore_initialized) {
        sem_destroy(&airport->available_runways);
    }

    if (airport->tower_event_initialized) {
        pthread_cond_destroy(&airport->tower_event);
    }

    if (airport->mutex_initialized) {
        pthread_mutex_destroy(&airport->mutex);
    }

    queue_destroy(&airport->emergency_landing_queue);
    queue_destroy(&airport->landing_queue);
    queue_destroy(&airport->takeoff_queue);

    free(airport->runways);
    airport->runways = NULL;
}

int airport_find_free_runway(const Airport *airport)
{
    if (airport == NULL) {
        return -1;
    }

    for (int i = 0; i < airport->config.runway_count; i++) {
        if (airport->runways[i].occupied_by < 0) {
            return i;
        }
    }

    return -1;
}

int airport_assign_runway(Airport *airport, int plane_id, RequestType operation)
{
    int runway_id;

    if (airport == NULL) {
        return -1;
    }

    runway_id = airport_find_free_runway(airport);
    if (runway_id < 0) {
        return -1;
    }

    airport->runways[runway_id].occupied_by = plane_id;
    airport->runways[runway_id].operation = operation;

    return runway_id;
}

int airport_release_runway(Airport *airport, int runway_id, int plane_id)
{
    if (airport == NULL || runway_id < 0 || runway_id >= airport->config.runway_count) {
        return -1;
    }

    if (airport->runways[runway_id].occupied_by != plane_id) {
        return -1;
    }

    airport->runways[runway_id].occupied_by = -1;

    return 0;
}
