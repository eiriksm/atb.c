#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5

typedef struct {
    int departure_times[MAX_DEPARTURES];
    int count;
} DepartureTimes;

// Schedule information for route 9
static const int route_9_schedule[][MAX_DEPARTURES] = {
    {600, 630, 700, 730, 800},  // Stop 1
    {605, 635, 705, 735, 805},  // Stop 2
    {610, 640, 710, 740, 810},  // Stop 3
    {615, 645, 715, 745, 815},  // Stop 4
    {620, 650, 720, 750, 820}   // Stop 5
};

// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char route, char stop_id);
DepartureTimes atb_get_next_departures(int timestamp, char route, char stop_id);
DepartureTimes atb_get_next_departures_route_9(int timestamp, char stop_id);

#endif

// Declare the atb_get_next_departure function
int atb_get_next_departure(int timestamp, char route, char stop_id);
