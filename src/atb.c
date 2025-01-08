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
