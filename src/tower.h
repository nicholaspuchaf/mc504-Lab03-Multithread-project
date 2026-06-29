#ifndef TOWER_H
#define TOWER_H

#include "airport.h"

typedef struct {
    Airport *airport;
    Airplane *airplanes;
} TowerThreadArgs;

void *tower_thread(void *arg);

#endif
