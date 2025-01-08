#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5

typedef struct {
    int departure_times[MAX_DEPARTURES];
    int count;
} DepartureTimes;


// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char route, char stop_id);
DepartureTimes atb_get_next_departures(int timestamp, char route, char stop_id);


#endif
