#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "atb.h"

int atb_get_next_departure(int timestamp, char route, char stop_id) {
    DepartureTimes departures = atb_get_next_departures(timestamp, route, stop_id);
    if (departures.count > 0) {
        return departures.departure_times[0];
    }
    return -1; // No next departure found
}

DepartureTimes atb_get_next_departures(int timestamp, char route, char stop_id) {
    DepartureTimes result;
    result.count = 0;

    if (route == '9') {
        return atb_get_next_departures_route_9(timestamp, stop_id);
    }

    return result;
}

DepartureTimes atb_get_next_departures_route_9(int timestamp, char stop_id) {
    DepartureTimes result;
    result.count = 0;

    if (stop_id != 1) {
        return result;
    }

    for (int i = 0; i < MAX_DEPARTURES; i++) {
        int departure_time;
        sscanf(route_9_schedule[0][i], "%d:%d", &departure_time);
        if (departure_time >= timestamp) {
            result.departure_times[result.count++] = departure_time;
        }
    }

    return result;
}
