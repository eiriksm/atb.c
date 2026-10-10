#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "atb.h"

/**
 * The following timegm stuff was copied directly from stack overflow.
 *
 * I renamed the function so we can actually use literally the same timegm
 * on all platforms, instead of subtle differences if we were so lucky that
 * timegm was in fact available.
 *
 * https://stackoverflow.com/a/58037981
 */

// Algorithm: http://howardhinnant.github.io/date_algorithms.html
int days_from_epoch(int y, int m, int d)
{
    y -= m <= 2;
    int era = y / 400;
    int yoe = y - era * 400;                                   // [0, 399]
    int doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
    int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;           // [0, 146096]
    return era * 146097 + doe - 719468;
}

// It  does not modify broken-down time
time_t my_timegm(struct tm const* t)
{
    int year = t->tm_year + 1900;
    int month = t->tm_mon;          // 0-11
    if (month > 11)
    {
        year += month / 12;
        month %= 12;
    }
    else if (month < 0)
    {
        int years_diff = (11 - month) / 12;
        year -= years_diff;
        month += 12 * years_diff;
    }
    int days_since_epoch = days_from_epoch(year, month + 1, t->tm_mday);

    return 60 * (60 * (24L * days_since_epoch + t->tm_hour) + t->tm_min) + t->tm_sec;
}

int is_dst_in_cet(const struct tm *time) {
    int year = time->tm_year + 1900;

    // Calculate last Sunday in March (DST starts)
    struct tm dst_start = {0};
    dst_start.tm_year = year - 1900;
    dst_start.tm_mon = 2; // March
    dst_start.tm_mday = 31; // Start with the last day of March
    dst_start.tm_hour = 2; // DST starts at 02:00
    mktime(&dst_start);

    // Move back to the previous Sunday if needed
    while (dst_start.tm_wday != 0) {
        dst_start.tm_mday--;
        mktime(&dst_start); // Normalize the structure
    }

    // Calculate last Sunday in October (DST ends)
    struct tm dst_end = {0};
    dst_end.tm_year = year - 1900;
    dst_end.tm_mon = 9; // October
    dst_end.tm_mday = 31; // Start with the last day of October
    dst_end.tm_hour = 3; // DST ends at 03:00
    mktime(&dst_end);

    // Move back to the previous Sunday if needed
    while (dst_end.tm_wday != 0) {
        dst_end.tm_mday--;
        mktime(&dst_end); // Normalize the structure
    }

    // Check if the given time is within the DST period
    time_t current = mktime((struct tm *)time);
    return (current >= mktime(&dst_start) && current < mktime(&dst_end));
}

int get_cet_offset_without_setenv(time_t utc_time) {
    struct tm *utc_tm = gmtime(&utc_time);
    int offset = 3600; // Standard CET offset is UTC+1

    // Adjust for DST
    if (is_dst_in_cet(utc_tm)) {
        offset += 3600; // Add 1 hour for DST
    }

    return offset;
}

/**
 * The departures of one CET/CEST day, from a given point in time.
 *
 * Times are kept as "local seconds": CET wall clock time counted as if it was
 * UTC, which keeps the arithmetic free of time zone handling.
 */
typedef struct {
    time_t day_start;
    time_t now;
    int day_of_week;
    // The earliest departures at or after now, in order.
    time_t departures[MAX_DEPARTURES];
    int count;
} AtbDay;

static void atb_day_init(AtbDay *day, int timestamp) {
    day->now = (time_t) timestamp + get_cet_offset_without_setenv((time_t) timestamp);
    day->day_start = day->now - day->now % 86400;
    // 1970-01-01 was a Thursday.
    day->day_of_week = (int) ((day->day_start / 86400 + 4) % 7);
    day->count = 0;
}

static void atb_day_add_departure(AtbDay *day, int minutes) {
    time_t departure = day->day_start + (time_t) minutes * 60;
    if (departure < day->now) {
        return;
    }
    if (day->count == MAX_DEPARTURES && departure >= day->departures[MAX_DEPARTURES - 1]) {
        return;
    }
    int i = day->count < MAX_DEPARTURES ? day->count++ : MAX_DEPARTURES - 1;
    while (i > 0 && day->departures[i - 1] > departure) {
        day->departures[i] = day->departures[i - 1];
        i--;
    }
    day->departures[i] = departure;
}

static ResultSet atb_day_result(const AtbDay *day) {
    ResultSet result;
    for (int i = 0; i < MAX_DEPARTURES; i++) {
        result.resultSet[i] = i < day->count
            ? (int) (day->departures[i] - get_cet_offset_without_setenv(day->departures[i]))
            : 0;
    }
    return result;
}

/**
 * Parses "HH:MM" (hours may go past 23 for departures after midnight) into
 * minutes since the start of the day. Returns a pointer past the time, or NULL
 * if there is no valid time.
 */
static const char *atb_parse_time(const char *s, int *minutes) {
    int hours = 0;
    int digits = 0;
    while (isdigit((unsigned char) *s) && digits < 2) {
        hours = hours * 10 + (*s++ - '0');
        digits++;
    }
    if (digits == 0 || *s != ':' || !isdigit((unsigned char) s[1]) || !isdigit((unsigned char) s[2])) {
        return NULL;
    }
    int mins = (s[1] - '0') * 10 + (s[2] - '0');
    if (mins > 59) {
        return NULL;
    }
    *minutes = hours * 60 + mins;
    return s + 3;
}

#ifndef ATB_NO_BUILTIN_SCHEDULES
int atb_get_next_departure(int timestamp, char* route, char* stop_id) {
    ResultSet departures = atb_get_next_departures(timestamp, route, stop_id);
    if (departures.resultSet[0] != 0) {
        return departures.resultSet[0];
    }
    return -1; // No next departure found
}

ResultSet atb_get_next_departures(int timestamp, char* route, char* stop_id) {
    // Calculate the number of elements in the schedules array
    int num_schedules = sizeof(schedules) / sizeof(schedules[0]);
    int num_stops = sizeof(stop_offsets) / sizeof(stop_offsets[0]);

    // First find the stop id, and its offset on the route.
    int stop_offset_in_minutes = 0;
    for (int i = 0; i < num_stops; i++) {
        if (strcmp(stop_offsets[i].stop_id, stop_id) == 0 && strcmp(stop_offsets[i].route, route) == 0) {
            stop_offset_in_minutes = stop_offsets[i].offset;
            break;
        }
    }

    AtbDay day;
    atb_day_init(&day, timestamp);
    int day_id = (day.day_of_week == ATB_SATURDAY || day.day_of_week == ATB_SUNDAY) ? day.day_of_week : ATB_WEEKDAY;

    for (int i = 0; i < num_schedules; i++) {
        if (strcmp(schedules[i].route, route) != 0 || schedules[i].day_id != day_id) {
            continue;
        }
        for (int j = 0; j < schedules[i].departureTimes.count; j++) {
            int minutes;
            if (atb_parse_time(schedules[i].departureTimes.departure_times[j], &minutes)) {
                atb_day_add_departure(&day, minutes + stop_offset_in_minutes);
            }
        }
        break; // Exit the outer loop once the route is found
    }

    return atb_day_result(&day);
}
#endif

/**
 * Skips whitespace, then null terminates and returns the next word. Returns
 * NULL when the string is used up.
 */
static char *atb_next_word(char **cursor) {
    char *start = *cursor;
    while (isspace((unsigned char) *start)) {
        start++;
    }
    if (*start == '\0') {
        *cursor = start;
        return NULL;
    }
    char *end = start;
    while (*end != '\0' && !isspace((unsigned char) *end)) {
        end++;
    }
    if (*end != '\0') {
        *end++ = '\0';
    }
    *cursor = end;
    return start;
}

/**
 * Reads the next line, with comments and surrounding whitespace removed.
 * Returns NULL at end of file.
 */
static char *atb_read_line(atb_line_reader_t reader, void *ctx, char *buf) {
    buf[0] = '\0';
    if (!reader(ctx, buf, ATB_FILE_LINE_MAX)) {
        return NULL;
    }
    buf[ATB_FILE_LINE_MAX - 1] = '\0';
    char *comment = strchr(buf, '#');
    if (comment != NULL) {
        *comment = '\0';
    }
    char *start = buf;
    while (isspace((unsigned char) *start)) {
        start++;
    }
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char) end[-1])) {
        *--end = '\0';
    }
    return start;
}

static void atb_copy_label(char *dest, const char *src, size_t size) {
    size_t i = 0;
    if (src != NULL) {
        for (; i < size - 1 && src[i] != '\0'; i++) {
            dest[i] = src[i];
        }
    }
    dest[i] = '\0';
}

// Parses the inside of an entry header like "[UGL 9]".
static void atb_parse_header(char *line, AtbEntryInfo *info) {
    char *cursor = line + 1;
    char *close = strchr(cursor, ']');
    if (close != NULL) {
        *close = '\0';
    }
    atb_copy_label(info->label, atb_next_word(&cursor), sizeof(info->label));
    atb_copy_label(info->route_label, atb_next_word(&cursor), sizeof(info->route_label));
}

/**
 * Reads lines until the header of entry number index (counting from 0).
 * Returns false if the file has fewer entries.
 */
static bool atb_seek_entry(atb_line_reader_t reader, void *ctx, int index, char *buf, AtbEntryInfo *info) {
    int current = -1;
    char *line;
    while ((line = atb_read_line(reader, ctx, buf)) != NULL) {
        if (line[0] == '[' && ++current == index) {
            if (info != NULL) {
                atb_parse_header(line, info);
            }
            return true;
        }
    }
    return false;
}

// Returns 0-6 (Sunday first) for a three letter day name, or -1.
static int atb_parse_day_name(const char *name, size_t len) {
    static const char *const day_names[] = {"sun", "mon", "tue", "wed", "thu", "fri", "sat"};
    if (len != 3) {
        return -1;
    }
    for (int day = 0; day < 7; day++) {
        if (tolower((unsigned char) name[0]) == day_names[day][0]
            && tolower((unsigned char) name[1]) == day_names[day][1]
            && tolower((unsigned char) name[2]) == day_names[day][2]) {
            return day;
        }
    }
    return -1;
}

/**
 * Parses day specs like "mon-fri", "sat,sun", "fri-mon" or "daily" into a bit
 * mask with bit 0 for Sunday. Returns 0 if this is not a day spec.
 */
static int atb_parse_day_mask(const char *spec) {
    if (strlen(spec) == 5 && strncmp(spec, "daily", 5) == 0) {
        return 0x7f;
    }
    int mask = 0;
    while (*spec != '\0') {
        size_t part_len = strcspn(spec, ",");
        const char *dash = memchr(spec, '-', part_len);
        int first = atb_parse_day_name(spec, dash ? (size_t) (dash - spec) : part_len);
        int last = dash ? atb_parse_day_name(dash + 1, part_len - (dash - spec) - 1) : first;
        if (first < 0 || last < 0) {
            return 0;
        }
        for (int day = first; ; day = (day + 1) % 7) {
            mask |= 1 << day;
            if (day == last) {
                break;
            }
        }
        spec += part_len;
        if (*spec == ',') {
            spec++;
        }
    }
    return mask;
}

/**
 * Adds the departures in one time token: either "HH:MM", or "HH:MM-HH:MM/N"
 * for every N minutes between the two times, both included.
 */
static void atb_add_time_token(AtbDay *day, const char *token, int offset) {
    int first, last, interval;
    const char *rest = atb_parse_time(token, &first);
    if (rest == NULL) {
        return;
    }
    if (*rest == '\0') {
        atb_day_add_departure(day, first + offset);
        return;
    }
    if (*rest != '-' || (rest = atb_parse_time(rest + 1, &last)) == NULL || *rest != '/') {
        return;
    }
    char *end;
    interval = (int) strtol(rest + 1, &end, 10);
    if (*end != '\0' || interval <= 0) {
        return;
    }
    for (int minutes = first; minutes <= last; minutes += interval) {
        atb_day_add_departure(day, minutes + offset);
    }
}

int atb_file_count_entries(atb_line_reader_t reader, void *ctx) {
    char buf[ATB_FILE_LINE_MAX];
    int count = 0;
    while (atb_seek_entry(reader, ctx, 0, buf, NULL)) {
        count++;
    }
    return count;
}

bool atb_file_get_entry(atb_line_reader_t reader, void *ctx, int index, AtbEntryInfo *info) {
    char buf[ATB_FILE_LINE_MAX];
    return atb_seek_entry(reader, ctx, index, buf, info);
}

ResultSet atb_file_next_departures(int timestamp, atb_line_reader_t reader, void *ctx, int index) {
    char buf[ATB_FILE_LINE_MAX];
    AtbDay day;
    atb_day_init(&day, timestamp);

    if (atb_seek_entry(reader, ctx, index, buf, NULL)) {
        int offset = 0;
        char *line;
        while ((line = atb_read_line(reader, ctx, buf)) != NULL && line[0] != '[') {
            char *cursor = line;
            char *keyword = atb_next_word(&cursor);
            if (keyword == NULL) {
                continue;
            }
            if (strcmp(keyword, "offset") == 0) {
                char *value = atb_next_word(&cursor);
                offset = value ? atoi(value) : 0;
                continue;
            }
            // Anything else that is not a day spec (route, stop, unknown
            // keywords) is informational, and skipped.
            if (!(atb_parse_day_mask(keyword) & (1 << day.day_of_week))) {
                continue;
            }
            char *token;
            while ((token = atb_next_word(&cursor)) != NULL) {
                atb_add_time_token(&day, token, offset);
            }
        }
    }

    return atb_day_result(&day);
}
