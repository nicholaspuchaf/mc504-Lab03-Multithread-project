#ifndef AIRPLANE_H
#define AIRPLANE_H

#include "airport.h"

typedef struct {
    Airport *airport;
    Airplane *airplane;
} AirplaneThreadArgs;

void *airplane_thread(void *arg);

#endif
