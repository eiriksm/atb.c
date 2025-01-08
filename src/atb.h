#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5

typedef struct {
    int departure_times[MAX_DEPARTURES];
    int count;
} DepartureTimes;

// Schedule information for route 9
static const char* route_9_schedule[][MAX_DEPARTURES] = {
    {"06:00", "06:30", "07:00", "07:30", "08:00"}  // Stop 1
};

// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char route, char stop_id);
DepartureTimes atb_get_next_departures(int timestamp, char route, char stop_id);
DepartureTimes atb_get_next_departures_route_9(int timestamp, char stop_id);

#endif
