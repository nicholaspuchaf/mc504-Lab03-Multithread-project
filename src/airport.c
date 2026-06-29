#include "airport.h"

#include <stdlib.h>

Config airport_default_config(void)
{
    Config config = {
        .total_planes = 10,
        .runway_count = 2,
        .max_queue_size = 32,
        .max_consecutive_landings = 3,
        .landing_count_before_takeoff = 0,
        .running = true,
    };

    return config;
}

int airport_init(Airport *airport, Config config)
{
    if (airport == NULL || config.runway_count <= 0 || config.max_queue_size <= 0) {
        return -1;
    }

    airport->config = config;
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

    if (pthread_mutex_init(&airport->mutex, NULL) != 0) {
        airport_destroy(airport);
        return -1;
    }

    if (pthread_cond_init(&airport->tower_event, NULL) != 0) {
        airport_destroy(airport);
        return -1;
    }

    if (sem_init(&airport->available_runways, 0, (unsigned int)config.runway_count) != 0) {
        airport_destroy(airport);
        return -1;
    }

    return 0;
}

void airport_destroy(Airport *airport)
{
    if (airport == NULL) {
        return;
    }

    sem_destroy(&airport->available_runways);
    pthread_cond_destroy(&airport->tower_event);
    pthread_mutex_destroy(&airport->mutex);
    queue_destroy(&airport->emergency_landing_queue);
    queue_destroy(&airport->landing_queue);
    queue_destroy(&airport->takeoff_queue);
    free(airport->runways);
    airport->runways = NULL;
}
