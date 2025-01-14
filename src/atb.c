#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "atb.h"

int atb_get_next_departure(int timestamp, char* route, char* stop_id) {
    ResultSet departures = atb_get_next_departures(timestamp, route, stop_id);
    if (sizeof(departures.resultSet) > 0) {
        return departures.resultSet[0];
    }
    return -1; // No next departure found
}

ResultSet atb_get_next_departures(int timestamp, char* route, char* stop_id) {
    ResultSet result;
    // Calculate the number of elements in the schedules array
    int num_schedules = sizeof(schedules) / sizeof(schedules[0]);
    int num_stops = sizeof(stop_offsets) / sizeof(stop_offsets[0]);

    int result_count = 0;

    // First find the stop id, and its offset on the route.
    int stop_offset_in_minutes = 0;
    for (int i = 0; i < num_stops; i++) {
        if (strcmp(stop_offsets[i].stop_id, stop_id) == 0 && strcmp(stop_offsets[i].route, route) == 0) {
            stop_offset_in_minutes = stop_offsets[i].offset;
            break;
        }
    }

    // Convert the integer to time_t
    time_t ttimestamp = (time_t) timestamp;
    struct tm *time_info = localtime(&ttimestamp);
        if (time_info == NULL) {
        fprintf(stderr, "Error: Could not convert timestamp.\n");
        return result;
    }

    int day_of_week = time_info->tm_wday;

    for (int i = 0; i < num_schedules; i++) {
        // Compare the route (string comparison).
        if (strcmp(schedules[i].route, route) == 0) {
            if (day_of_week == ATB_SATURDAY || day_of_week == ATB_SUNDAY) {
                if (schedules[i].day_id != day_of_week) {
                    continue;
                }
            }
            else {
                if (schedules[i].day_id != ATB_WEEKDAY) {
                    continue;
                }
            }
            for (int j = 0; j < schedules[i].departureTimes.count; j++) {
                // Create a timestamp from the string, and make it so the timestamp represents the same day.
                char *hour, *minute;
                char *departure_time = schedules[i].departureTimes.departure_times[j];
                char buffer[6]; // Make sure the buffer is large enough for your strings
                strncpy(buffer, departure_time, sizeof(buffer) - 1);
                buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination
                // Split the buffer using strtok
                hour = strtok(buffer, ":");
                minute = strtok(NULL, ":");
                // Convert hour and minute to integers
                int new_hour = atoi(hour);
                int new_minute = atoi(minute);

                // Update time_info with new hour and minute
                time_info->tm_hour = new_hour;
                time_info->tm_min = new_minute;
                time_info->tm_sec = 0; // Set seconds to 0

                // Convert back to Unix timestamp
                time_t new_timestamp = mktime(time_info);
                new_timestamp += stop_offset_in_minutes * 60;
                // Check if the departure time is after the given timestamp
                if (new_timestamp >= timestamp) {
                    result.resultSet[result_count] = new_timestamp;
                    result_count++;
                }
                if (result_count == MAX_DEPARTURES) {
                    break;
                }

            }
            break; // Exit the outer loop once the route is found
        }
    }

    return result;
}
