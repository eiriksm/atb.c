#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5

typedef struct {
    char* departure_times[MAX_DEPARTURES];
    int count;
} DepartureTimes;

typedef struct {
    int resultSet[MAX_DEPARTURES];
} ResultSet;

typedef struct {
    char* stop_id;
    char* route;
    int offset;
} StopOffset;

typedef struct route {
    char* route;
    int day_id;
    DepartureTimes departureTimes;
} Route;

static const Route schedules[1] = {
    {"9", 5, {{"14:57", "15:12", "15:27", "15:42", "15:57"}, 5}}
};

static const StopOffset stop_offsets[1] = {
    {"71779", "9", 5}
};


// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char* route, char* stop_id);
ResultSet atb_get_next_departures(int timestamp, char* route, char* stop_id);

#endif
