#include "airport.h"
#include "airplane.h"
#include "render.h"
#include "tower.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void sleep_ms(int milliseconds)
{
    struct timespec duration = {
        .tv_sec = milliseconds / 1000,
        .tv_nsec = (long)(milliseconds % 1000) * 1000000L,
    };

    nanosleep(&duration, NULL);
}

static void print_usage(const char *program_name)
{
    printf("Uso: %s [opcoes]\n\n", program_name);
    printf("Opcoes:\n");
    printf("  --planes N             Numero total de avioes (padrao: 10)\n");
    printf("  --runways N            Numero de pistas (padrao: 2)\n");
    printf("  --queue-size N         Tamanho maximo das filas (padrao: 32)\n");
    printf("  --landing-prob N       Probabilidade de pouso em porcentagem (padrao: 65)\n");
    printf("  --emergency-prob N     Probabilidade de emergencia entre pousos (padrao: 20)\n");
    printf("  --runway-time-ms N     Tempo de uso da pista em ms (padrao: 700)\n");
    printf("  --arrival-delay-ms N   Intervalo entre chegadas em ms (padrao: 120)\n");
    printf("  --max-landings N       Maximo de pousos consecutivos (padrao: 3)\n");
    printf("  --no-animation         Desativa animacao e mostra logs lineares\n");
    printf("  --help                 Mostra esta ajuda\n");
}

static int parse_int_arg(const char *value, int *output)
{
    char *end = NULL;
    long parsed;

    if (value == NULL || output == NULL) {
        return -1;
    }

    errno = 0;
    parsed = strtol(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed < 0 || parsed > 1000000L) {
        return -1;
    }

    *output = (int)parsed;
    return 0;
}

static int parse_args(int argc, char **argv, Config *config)
{
    for (int i = 1; i < argc; i++) {
        int *target = NULL;

        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 1;
        }

        if (strcmp(argv[i], "--no-animation") == 0) {
            config->no_animation = true;
            continue;
        }

        if (i + 1 >= argc) {
            fprintf(stderr, "Opcao sem valor: %s\n", argv[i]);
            return -1;
        }

        if (strcmp(argv[i], "--planes") == 0) {
            target = &config->total_planes;
        } else if (strcmp(argv[i], "--runways") == 0) {
            target = &config->runway_count;
        } else if (strcmp(argv[i], "--queue-size") == 0) {
            target = &config->max_queue_size;
        } else if (strcmp(argv[i], "--landing-prob") == 0) {
            target = &config->landing_probability;
        } else if (strcmp(argv[i], "--emergency-prob") == 0) {
            target = &config->emergency_probability;
        } else if (strcmp(argv[i], "--runway-time-ms") == 0) {
            target = &config->runway_time_ms;
        } else if (strcmp(argv[i], "--arrival-delay-ms") == 0) {
            target = &config->arrival_delay_ms;
        } else if (strcmp(argv[i], "--max-landings") == 0) {
            target = &config->max_consecutive_landings;
        } else {
            fprintf(stderr, "Opcao desconhecida: %s\n", argv[i]);
            return -1;
        }

        if (parse_int_arg(argv[i + 1], target) != 0) {
            fprintf(stderr, "Valor invalido para %s: %s\n", argv[i], argv[i + 1]);
            return -1;
        }

        i++;
    }

    if (config->total_planes <= 0 ||
        config->runway_count <= 0 ||
        config->max_queue_size <= 0 ||
        config->max_consecutive_landings <= 0 ||
        config->runway_time_ms < 0 ||
        config->arrival_delay_ms < 0 ||
        config->landing_probability < 0 ||
        config->landing_probability > 100 ||
        config->emergency_probability < 0 ||
        config->emergency_probability > 100) {
        fprintf(stderr, "Configuracao invalida. Use --help para ver os parametros.\n");
        return -1;
    }

    return 0;
}

static int setup_airplane(Airplane *airplane, int id, const Config *config)
{
    bool wants_landing;

    if (pthread_cond_init(&airplane->authorized, NULL) != 0) {
        return -1;
    }

    wants_landing = (rand() % 100) < config->landing_probability;
    airplane->id = id;
    airplane->request = wants_landing ? REQUEST_LANDING : REQUEST_TAKEOFF;
    airplane->state = airplane->request == REQUEST_LANDING
                          ? PLANE_WAITING_LANDING
                          : PLANE_WAITING_TAKEOFF;
    airplane->emergency = airplane->request == REQUEST_LANDING &&
                          (rand() % 100) < config->emergency_probability;
    airplane->runway_assigned = -1;
    airplane->cleared = false;
    airplane->condition_initialized = true;

    return 0;
}

int main(int argc, char **argv)
{
    Airport airport;
    Config config = airport_default_config();
    Airplane *airplanes = NULL;
    AirplaneThreadArgs *airplane_args = NULL;
    pthread_t *airplane_threads = NULL;
    pthread_t tower;
    pthread_t renderer;
    TowerThreadArgs tower_args;
    RenderThreadArgs render_args;
    int created_planes = 0;
    int initialized_planes = 0;
    bool tower_created = false;
    bool renderer_created = false;
    int exit_code = 0;
    int parse_result;

    parse_result = parse_args(argc, argv, &config);
    if (parse_result > 0) {
        return 0;
    }

    if (parse_result < 0) {
        return 1;
    }

    srand((unsigned int)time(NULL));

    if (airport_init(&airport, config) != 0) {
        fprintf(stderr, "Erro ao inicializar o aeroporto.\n");
        return 1;
    }

    airplanes = calloc((size_t)config.total_planes, sizeof(Airplane));
    airplane_args = calloc((size_t)config.total_planes, sizeof(AirplaneThreadArgs));
    airplane_threads = calloc((size_t)config.total_planes, sizeof(pthread_t));

    if (airplanes == NULL || airplane_args == NULL || airplane_threads == NULL) {
        fprintf(stderr, "Erro ao alocar estruturas dos avioes.\n");
        exit_code = 1;
        goto cleanup;
    }

    for (int i = 0; i < config.total_planes; i++) {
        if (setup_airplane(&airplanes[i], i, &config) != 0) {
            fprintf(stderr, "Erro ao inicializar condicao do aviao A%02d.\n", i);
            exit_code = 1;
            goto cleanup;
        }

        initialized_planes++;
    }

    tower_args.airport = &airport;
    tower_args.airplanes = airplanes;
    render_args.airport = &airport;

    if (pthread_create(&tower, NULL, tower_thread, &tower_args) != 0) {
        fprintf(stderr, "Erro ao criar thread da torre.\n");
        exit_code = 1;
        goto cleanup;
    }
    tower_created = true;

    if (!config.no_animation) {
        if (pthread_create(&renderer, NULL, render_thread, &render_args) != 0) {
            fprintf(stderr, "Erro ao criar thread de renderizacao.\n");
            exit_code = 1;
            goto cleanup;
        }
        renderer_created = true;
    }

    for (int i = 0; i < config.total_planes; i++) {
        airplane_args[i].airport = &airport;
        airplane_args[i].airplane = &airplanes[i];

        if (pthread_create(&airplane_threads[i], NULL, airplane_thread, &airplane_args[i]) != 0) {
            fprintf(stderr, "Erro ao criar thread do aviao A%02d.\n", i);
            exit_code = 1;
            break;
        }

        created_planes++;
        sleep_ms(config.arrival_delay_ms);
    }

    for (int i = 0; i < created_planes; i++) {
        pthread_join(airplane_threads[i], NULL);
    }

    if (tower_created) {
        pthread_mutex_lock(&airport.mutex);
        airport.config.running = false;
        pthread_cond_signal(&airport.tower_event);
        pthread_mutex_unlock(&airport.mutex);

        pthread_join(tower, NULL);
        tower_created = false;
    }

    if (renderer_created) {
        pthread_join(renderer, NULL);
        renderer_created = false;
    }

    render_airport(&airport);

cleanup:
    if (tower_created && exit_code != 0) {
        pthread_mutex_lock(&airport.mutex);
        airport.config.running = false;
        pthread_cond_broadcast(&airport.tower_event);
        pthread_mutex_unlock(&airport.mutex);
        pthread_join(tower, NULL);
    }

    if (renderer_created && exit_code != 0) {
        pthread_join(renderer, NULL);
    }

    if (airplanes != NULL) {
        for (int i = 0; i < initialized_planes; i++) {
            if (airplanes[i].condition_initialized) {
                pthread_cond_destroy(&airplanes[i].authorized);
            }
        }
    }

    free(airplane_threads);
    free(airplane_args);
    free(airplanes);
    airport_destroy(&airport);

    return exit_code;
}
