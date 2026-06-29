#include "render.h"

#include <stdio.h>

static const char *request_type_label(RequestType request)
{
    return request == REQUEST_LANDING ? "POUSO" : "DECOLAGEM";
}

void render_airport(const Airport *airport)
{
    if (airport == NULL) {
        return;
    }

    printf("\033[2J\033[H");
    printf("AEROPORTO - TORRE DE CONTROLE\n\n");
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

    printf("\nFilas:\n");
    printf("  Emergencia: %d avioes\n", airport->emergency_landing_queue.size);
    printf("  Pouso:      %d avioes\n", airport->landing_queue.size);
    printf("  Decolagem:  %d avioes\n", airport->takeoff_queue.size);
}
