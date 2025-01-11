#ifndef ATB_H  // Include guard to prevent multiple inclusions
#define ATB_H

#define MAX_DEPARTURES 5
#define ATB_WEEKDAY 1
#define ATB_SATURDAY 6
#define ATB_SUNDAY 0

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
    {"9", ATB_WEEKDAY, {{
        // Every 15 minutes from 05:57 to 18:12
        "05:57", "06:12", "06:27", "06:42", "06:57", "07:12", "07:27", "07:42", "07:57",
        "08:12", "08:27", "08:42", "08:57", "09:12", "09:27", "09:42", "09:57",
        "10:12", "10:27", "10:42", "10:57", "11:12", "11:27", "11:42", "11:57",
        "12:12", "12:27", "12:42", "12:57", "13:12", "13:27", "13:42", "13:57",
        "14:12", "14:27", "14:42", "14:57", "15:12", "15:27", "15:42", "15:57",
        "16:12", "16:27", "16:42", "16:57", "17:12", "17:27", "17:42", "17:57",
        "18:12",
        // Every 30 minutes from 18:42 to 23:42
        "18:42", "19:12", "19:42", "20:12", "20:42", "21:12", "21:42", "22:12",
        "22:42", "23:12", "23:42"
    }, .count = 61}},
    {"9", ATB_SATURDAY, {{
        // 07:12 to 09:12 every 30 minutes
        "07:12", "07:42", "08:12", "08:42", "09:12",

        // 09:27 to 18:12 every 15 minutes
        "09:27", "09:42", "09:57", "10:12", "10:27", "10:42", "10:57", "11:12", "11:27", "11:42",
        "11:57", "12:12", "12:27", "12:42", "12:57", "13:12", "13:27", "13:42", "13:57", "14:12",
        "14:27", "14:42", "14:57", "15:12", "15:27", "15:42", "15:57", "16:12", "16:27", "16:42",
        "16:57", "17:12", "17:27", "17:42", "18:12",

        // 18:42 to 23:42 every 30 minutes
        "18:42", "19:12", "19:42", "20:12", "20:42", "21:12", "21:42", "22:12", "22:42", "23:12", "23:42"
    }, 51}},
    {"9", ATB_SUNDAY, {{
        // Every 30 minutes from 09:12 to 23:42
        "09:12", "09:42", "10:12", "10:42", "11:12", "11:42", "12:12", "12:42",
        "13:12", "13:42", "14:12", "14:42", "15:12", "15:42", "16:12", "16:42",
        "17:12", "17:42", "18:12", "18:42", "19:12", "19:42", "20:12", "20:42",
        "21:12", "21:42", "22:12", "22:42", "23:12", "23:42"
    }, 30}},
};

static const StopOffset stop_offsets[] = {
    {"71779", "9", 5},
};


// Function prototypes (declarations)
int atb_get_next_departure(int timestamp, char* route, char* stop_id);
ResultSet atb_get_next_departures(int timestamp, char* route, char* stop_id);

#endif
