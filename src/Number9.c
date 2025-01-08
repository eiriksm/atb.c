#include <string.h>

#include "atb.h"

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
