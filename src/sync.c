#include "sync.h"

int sync_has_waiting_planes(const Airport *airport)
{
    if (airport == NULL) {
        return 0;
    }

    return airport->emergency_landing_queue.size > 0 ||
           airport->landing_queue.size > 0 ||
           airport->takeoff_queue.size > 0;
}
