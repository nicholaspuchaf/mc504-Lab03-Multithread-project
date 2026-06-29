#include "render.h"

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

static const char *request_type_label(RequestType request)
{
    return request == REQUEST_LANDING ? "POUSO" : "DECOLAGEM";
}

static void print_separator(const char *title)
{
    printf("\n+---------------- %s ----------------+\n\n", title);
}

static void print_queue_slots(const char *label, const Queue *queue, int mark_emergency)
{
    const int visible_slots_limit = 10;
    int visible_slots = queue->capacity < visible_slots_limit ? queue->capacity : visible_slots_limit;

    printf("  %s %d/%d\n", label, queue->size, queue->capacity);
    printf("  ");

    for (int i = 0; i < visible_slots; i++) {
        if (i < queue->size) {
            int index = (queue->head + i) % queue->capacity;
            printf("[%cA%02d ]", mark_emergency ? '!' : ' ', queue->items[index]);
        } else {
            printf("[ ... ]");
        }
    }

    if (queue->capacity > visible_slots) {
        printf(" ... +%d lugares", queue->capacity - visible_slots);
    }

    printf("\n\n");
}

void render_airport(const Airport *airport)
{
    if (airport == NULL) {
        return;
    }

    if (!airport->config.no_animation) {
        printf("\033[2J\033[H");
    }

    printf("+=================================================+\n");
    printf("|          AEROPORTO - TORRE DE CONTROLE          |\n");
    printf("+=================================================+\n\n");

    print_separator("PARAMETROS");
    printf("Parametros:\n");
    printf("  Avioes: %d | Pistas: %d | Fila max: %d\n",
           airport->config.total_planes,
           airport->config.runway_count,
           airport->config.max_queue_size);
    printf("  Prob. pouso: %d%% | Prob. emergencia: %d%%\n",
           airport->config.landing_probability,
           airport->config.emergency_probability);
    printf("  Tempo pista: %d ms | Intervalo chegada: %d ms\n",
           airport->config.runway_time_ms,
           airport->config.arrival_delay_ms);
    printf("  Max. pousos consecutivos: %d\n\n",
           airport->config.max_consecutive_landings);

    print_separator("PISTAS");
    printf("Pistas:\n");

    for (int i = 0; i < airport->config.runway_count; i++) {
        const Runway *runway = &airport->runways[i];

        if (runway->occupied_by < 0) {
            printf("  Pista %d: [LIVRE]\n", runway->id);
        } else {
            printf("  Pista %d: [A%02d %s]\n",
                   runway->id,
                   runway->occupied_by,
                   request_type_label(runway->operation));
        }
    }

    print_separator("FILAS");
    print_queue_slots("Emergencia:", &airport->emergency_landing_queue, 1);
    print_queue_slots("Pouso:", &airport->landing_queue, 0);
    print_queue_slots("Decolagem:", &airport->takeoff_queue, 0);

    print_separator("ESTATISTICAS");
    printf("  Concluidos: %d/%d\n", airport->completed_planes, airport->config.total_planes);
    printf("  Pousos: %d | Decolagens: %d | Emergencias: %d\n",
           airport->landings_completed,
           airport->takeoffs_completed,
           airport->emergencies_completed);

    print_separator("POLITICA");
    printf("  Pousos consecutivos: %d/%d\n",
           airport->config.landing_count_before_takeoff,
           airport->config.max_consecutive_landings);
}

void *render_thread(void *arg)
{
    RenderThreadArgs *args = arg;

    if (args == NULL || args->airport == NULL) {
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&args->airport->mutex);
        render_airport(args->airport);

        if (!args->airport->config.running &&
            args->airport->completed_planes >= args->airport->config.total_planes) {
            pthread_mutex_unlock(&args->airport->mutex);
            break;
        }

        pthread_mutex_unlock(&args->airport->mutex);
        sleep_ms(120);
    }

    return NULL;
}
