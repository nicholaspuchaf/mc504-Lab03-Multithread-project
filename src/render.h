#ifndef RENDER_H
#define RENDER_H

#include "airport.h"

typedef struct {
    Airport *airport;
} RenderThreadArgs;

void render_airport(const Airport *airport);
void *render_thread(void *arg);

#endif
