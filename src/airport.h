#ifndef AIRPORT_H
#define AIRPORT_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "queue.h"

typedef enum {
    REQUEST_LANDING,
    REQUEST_TAKEOFF
} RequestType;

typedef enum {
    PLANE_WAITING_LANDING,
    PLANE_WAITING_TAKEOFF,
    PLANE_LANDING,
    PLANE_TAKING_OFF,
    PLANE_DONE,
    PLANE_LEFT
} PlaneState;

typedef struct {
    int id;
    RequestType request;
    PlaneState state;
    bool emergency;
    int runway_assigned;
    pthread_cond_t authorized;
    bool condition_initialized;
    bool cleared;
} Airplane;

typedef struct {
    int id;
    int occupied_by;
    RequestType operation;
} Runway;

typedef struct {
    int total_planes;
    int runway_count;
    int max_queue_size;
    int max_consecutive_landings;
    int landing_count_before_takeoff;
    int runway_time_ms;
    int arrival_delay_ms;
    int landing_probability;
    int emergency_probability;
    bool no_animation;
    bool running;
} Config;

typedef struct {
    Config config;
    Runway *runways;
    Queue emergency_landing_queue;
    Queue landing_queue;
    Queue takeoff_queue;
    pthread_mutex_t mutex;
    pthread_cond_t tower_event;
    sem_t available_runways;
    int completed_planes;
    int landings_completed;
    int takeoffs_completed;
    int emergencies_completed;
    bool queues_initialized;
    bool mutex_initialized;
    bool tower_event_initialized;
    bool semaphore_initialized;
} Airport;

int airport_init(Airport *airport, Config config);
void airport_destroy(Airport *airport);
Config airport_default_config(void);
int airport_find_free_runway(const Airport *airport);
int airport_assign_runway(Airport *airport, int plane_id, RequestType operation);
int airport_release_runway(Airport *airport, int runway_id, int plane_id);

#endif
