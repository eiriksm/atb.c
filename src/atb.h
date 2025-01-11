#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5
#define ATB_WEEKDAY 0
#define ATB_SATURDAY 6
#define ATB_SUNDAY 7

typedef struct {
    char* departure_times[100];
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

static const Route schedules[] = {
    {"9", ATB_SATURDAY, {{"13:42", "13:57", "14:12", "14:27", "14:42", "14:57", "15:12", "15:27", "15:42", "15:57"}, 9}},
    {"9", ATB_WEEKDAY, {{"13:42", "13:57", "14:12", "14:27", "14:42", "14:57", "15:12", "15:27", "15:42", "15:57"}, 9}},
};

static const StopOffset stop_offsets[] = {
    {"71779", "9", 5},
};


// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char* route, char* stop_id);
ResultSet atb_get_next_departures(int timestamp, char* route, char* stop_id);

#endif
