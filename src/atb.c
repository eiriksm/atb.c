#include <stddef.h>
#include <stdlib.h>

#include "atb.h"

int atb_get_next_departure(int timestamp, char route, char stop_id) {
  return 0;
}

DepartureTimes atb_get_next_departures(int timestamp, char route, char stop_id) {
  DepartureTimes result;
  // In the other file "Number9" I have some departure times. So if the route is 9, then we
  // get them from there.
  if (route == '9') {
    return atb_get_next_departures_route_9(timestamp, stop_id);
  }
  return result;
}

DepartureTimes atb_get_next_departures_route_9(int timestamp, char stop_id) {
  DepartureTimes result;
  result.count = 0;

  if (stop_id < 1 || stop_id > 5) {
    return result;
  }

  for (int i = 0; i < MAX_DEPARTURES; i++) {
    if (route_9_schedule[stop_id - 1][i] >= timestamp) {
      result.departure_times[result.count++] = route_9_schedule[stop_id - 1][i];
    }
  }

  return result;
}
