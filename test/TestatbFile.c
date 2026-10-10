#include "unity.h"
#include "atb.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    const char *text;
    size_t pos;
} StringReader;

static bool string_reader(void *ctx, char *buf, int len) {
    StringReader *reader = (StringReader *) ctx;
    if (reader->text[reader->pos] == '\0') {
        return false;
    }
    int i = 0;
    while (reader->text[reader->pos] != '\0' && reader->text[reader->pos] != '\n') {
        if (i < len - 1) {
            buf[i++] = reader->text[reader->pos];
        }
        reader->pos++;
    }
    if (reader->text[reader->pos] == '\n') {
        reader->pos++;
    }
    buf[i] = '\0';
    return true;
}

static ResultSet next_departures(const char *text, int timestamp, int index) {
    StringReader reader = {text, 0};
    return atb_file_next_departures(timestamp, string_reader, &reader, index);
}

// A timestamp for a CET (winter time) wall clock time, in January 2025.
// January 1st 2025 was a Wednesday.
static int cet_january(int day, int hour, int minute) {
    return 1735689600 - 3600 + (day - 1) * 86400 + hour * 3600 + minute * 60;
}

static const char EXAMPLE[] =
    "# Example schedules\n"
    "\n"
    "[UGL 9]   # Ugla, tram towards Sentrum\n"
    "route   09_2\n"
    "stop    71779\n"
    "offset  5\n"
    "mon-fri 05:57-18:12/15 18:42-23:42/30\n"
    "sat     07:12-08:42/30 09:12-18:12/15 18:42-23:42/30\n"
    "sun     09:12-23:42/30\n"
    "\n"
    "[STO 9]\n"
    "route   09_1\n"
    "stop    74061\n"
    "mon-fri 06:30-18:45/15 19:15-24:15/30\n"
    "sat     07:45-09:45/30 10:00-18:45/15 19:15-24:15/30\n"
    "sun     09:45-24:15/30\n"
    "\n"
    "  [  LONGLABEL 123  ]\n"
    "SAT,sun 10:00\n"
    "fri-mon 12:00 # wraps around the weekend\n"
    "mon-fri 08:00\n"
    "mon-fri 07:30 # continuing a day line, out of order\n"
    "tue     9:05\n"
    "daily   23:59\n"
    "offset  10\n"
    "wed     11:00\n"
    "bogus   06:00\n"
    "wed     5:61 abc 06:00- 06:00-07:00 06:00-07:00/0 06:00-07:00/x 13:00-12:00/5\n";

void setUp(void) {
}

void tearDown(void) {
}

void test_count_entries(void) {
    StringReader reader = {EXAMPLE, 0};
    TEST_ASSERT_EQUAL_INT(3, atb_file_count_entries(string_reader, &reader));

    StringReader empty = {"# Nothing here\nroute 09_2\n", 0};
    TEST_ASSERT_EQUAL_INT(0, atb_file_count_entries(string_reader, &empty));
}

void test_get_entry(void) {
    AtbEntryInfo info;
    StringReader reader = {EXAMPLE, 0};
    TEST_ASSERT_TRUE(atb_file_get_entry(string_reader, &reader, 0, &info));
    TEST_ASSERT_EQUAL_STRING("UGL", info.label);
    TEST_ASSERT_EQUAL_STRING("9", info.route_label);

    reader.pos = 0;
    TEST_ASSERT_TRUE(atb_file_get_entry(string_reader, &reader, 1, &info));
    TEST_ASSERT_EQUAL_STRING("STO", info.label);
    TEST_ASSERT_EQUAL_STRING("9", info.route_label);

    // Labels are cut to fit the display.
    reader.pos = 0;
    TEST_ASSERT_TRUE(atb_file_get_entry(string_reader, &reader, 2, &info));
    TEST_ASSERT_EQUAL_STRING("LON", info.label);
    TEST_ASSERT_EQUAL_STRING("12", info.route_label);

    reader.pos = 0;
    TEST_ASSERT_FALSE(atb_file_get_entry(string_reader, &reader, 3, &info));

    StringReader no_route = {"[ABC]\n", 0};
    TEST_ASSERT_TRUE(atb_file_get_entry(string_reader, &no_route, 0, &info));
    TEST_ASSERT_EQUAL_STRING("ABC", info.label);
    TEST_ASSERT_EQUAL_STRING("", info.route_label);
}

void test_ranges_and_offset(void) {
    // Wednesday 15:00:40. Departures every 15 minutes at :57, :12, ... plus 5
    // minutes offset.
    ResultSet result = next_departures(EXAMPLE, cet_january(29, 15, 0) + 40, 0);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 15, 2), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 15, 17), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 15, 32), result.resultSet[2]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 15, 47), result.resultSet[3]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 16, 2), result.resultSet[4]);

    // A departure at exactly now is still included.
    result = next_departures(EXAMPLE, cet_january(29, 15, 2), 0);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 15, 2), result.resultSet[0]);

    // Crossing from one range to the next: 18:12 + 5 is the last one every
    // 15 minutes, then every 30.
    result = next_departures(EXAMPLE, cet_january(29, 18, 10), 0);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 18, 17), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 18, 47), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 19, 17), result.resultSet[2]);
}

void test_after_midnight(void) {
    // Wednesday 23:50, the last tram at 24:15 is in the small hours of Thursday.
    ResultSet result = next_departures(EXAMPLE, cet_january(29, 23, 50), 1);
    TEST_ASSERT_EQUAL_INT(cet_january(30, 0, 15), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[1]);
}

void test_no_departures_left(void) {
    // Sunday after the last tram.
    ResultSet result = next_departures(EXAMPLE, cet_january(26, 23, 50), 0);
    for (int i = 0; i < MAX_DEPARTURES; i++) {
        TEST_ASSERT_EQUAL_INT(0, result.resultSet[i]);
    }

    // An entry that does not exist.
    result = next_departures(EXAMPLE, cet_january(29, 12, 0), 7);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[0]);
}

void test_day_specs(void) {
    // Saturday: "SAT,sun", "fri-mon" and "daily".
    ResultSet result = next_departures(EXAMPLE, cet_january(25, 0, 0), 2);
    TEST_ASSERT_EQUAL_INT(cet_january(25, 10, 0), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(25, 12, 0), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(cet_january(25, 23, 59), result.resultSet[2]);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[3]);

    // Monday: two "mon-fri" lines merged in order, then "fri-mon" and "daily".
    result = next_departures(EXAMPLE, cet_january(27, 0, 0), 2);
    TEST_ASSERT_EQUAL_INT(cet_january(27, 7, 30), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(27, 8, 0), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(cet_january(27, 12, 0), result.resultSet[2]);
    TEST_ASSERT_EQUAL_INT(cet_january(27, 23, 59), result.resultSet[3]);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[4]);

    // Tuesday: single digit hours are fine.
    result = next_departures(EXAMPLE, cet_january(28, 8, 30), 2);
    TEST_ASSERT_EQUAL_INT(cet_january(28, 9, 5), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(28, 23, 59), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[2]);
}

void test_offset_applies_to_following_lines_and_bad_input_is_skipped(void) {
    // Wednesday. The offset only shifts the line after it, the "bogus" line
    // is ignored, and none of the broken time tokens add anything.
    ResultSet result = next_departures(EXAMPLE, cet_january(29, 9, 0), 2);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 11, 10), result.resultSet[0]);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 23, 59), result.resultSet[1]);
    TEST_ASSERT_EQUAL_INT(0, result.resultSet[2]);
}

void test_offset_resets_between_entries(void) {
    static const char TEXT[] =
        "[A 1]\n"
        "offset 30\n"
        "daily 10:00\n"
        "[B 2]\n"
        "daily 10:00\n";
    ResultSet result = next_departures(TEXT, cet_january(29, 9, 0), 1);
    TEST_ASSERT_EQUAL_INT(cet_january(29, 10, 0), result.resultSet[0]);
}

static void assert_same_as_builtin(const char *text, int index, char *route, char *stop_id, int start) {
    char message[64];
    // Every 7 minutes for two weeks.
    for (int timestamp = start; timestamp < start + 14 * 86400; timestamp += 7 * 60) {
        ResultSet expected = atb_get_next_departures(timestamp, route, stop_id);
        ResultSet actual = next_departures(text, timestamp, index);
        snprintf(message, sizeof(message), "%s at %s, timestamp %d", route, stop_id, timestamp);
        TEST_ASSERT_EQUAL_INT_ARRAY_MESSAGE(expected.resultSet, actual.resultSet, MAX_DEPARTURES, message);
    }
}

void test_hand_written_ranges_match_builtin_schedules(void) {
    assert_same_as_builtin(EXAMPLE, 0, "09_2", "71779", cet_january(20, 0, 0));
    assert_same_as_builtin(EXAMPLE, 1, "09_1", "74061", cet_january(20, 0, 0));
}

// Writes every built in stop and route as a schedule file, one time at a time.
static void builtin_schedules_as_file(char *text, size_t size) {
    int num_schedules = sizeof(schedules) / sizeof(schedules[0]);
    int num_stops = sizeof(stop_offsets) / sizeof(stop_offsets[0]);
    size_t used = 0;
    for (int i = 0; i < num_stops; i++) {
        used += snprintf(text + used, size - used, "[S%d R]\nroute %s\nstop %s\noffset %d\n",
                         i, stop_offsets[i].route, stop_offsets[i].stop_id, stop_offsets[i].offset);
        for (int j = 0; j < num_schedules; j++) {
            if (strcmp(schedules[j].route, stop_offsets[i].route) != 0) {
                continue;
            }
            const char *days = schedules[j].day_id == ATB_WEEKDAY ? "mon-fri"
                : schedules[j].day_id == ATB_SATURDAY ? "sat" : "sun";
            for (int k = 0; k < schedules[j].departureTimes.count; k++) {
                // Ten times per line, to stay within the line length limit.
                if (k % 10 == 0) {
                    used += snprintf(text + used, size - used, "%s%s", k ? "\n" : "", days);
                }
                used += snprintf(text + used, size - used, " %s", schedules[j].departureTimes.departure_times[k]);
            }
            used += snprintf(text + used, size - used, "\n");
        }
        TEST_ASSERT_LESS_THAN(size, used);
    }
}

void test_builtin_schedules_as_file_match_builtin_schedules(void) {
    static char text[32 * 1024];
    builtin_schedules_as_file(text, sizeof(text));
    int num_stops = sizeof(stop_offsets) / sizeof(stop_offsets[0]);
    for (int i = 0; i < num_stops; i++) {
        // A winter and a summer time fortnight.
        assert_same_as_builtin(text, i, stop_offsets[i].route, stop_offsets[i].stop_id, cet_january(20, 0, 0));
        assert_same_as_builtin(text, i, stop_offsets[i].route, stop_offsets[i].stop_id, 1748815200);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_count_entries);
    RUN_TEST(test_get_entry);
    RUN_TEST(test_ranges_and_offset);
    RUN_TEST(test_after_midnight);
    RUN_TEST(test_no_departures_left);
    RUN_TEST(test_day_specs);
    RUN_TEST(test_offset_applies_to_following_lines_and_bad_input_is_skipped);
    RUN_TEST(test_offset_resets_between_entries);
    RUN_TEST(test_hand_written_ranges_match_builtin_schedules);
    RUN_TEST(test_builtin_schedules_as_file_match_builtin_schedules);
    return UNITY_END();
}
