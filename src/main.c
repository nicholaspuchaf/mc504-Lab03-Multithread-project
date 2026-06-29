#include "airport.h"
#include "render.h"

#include <stdio.h>

int main(void)
{
    Airport airport;
    Config config = airport_default_config();

    if (airport_init(&airport, config) != 0) {
        fprintf(stderr, "Erro ao inicializar o aeroporto.\n");
        return 1;
    }

    render_airport(&airport);
    airport_destroy(&airport);

    return 0;
}
