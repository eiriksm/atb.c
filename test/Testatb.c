#include "unity.h"
#include "atb.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void setUp(void) {
}

void tearDown(void) {
}

void test_get_next_departures_route_9(void) {
    ResultSet result;

    // Test for stop 71779. This is a Wednesday at 15:00:40. The next departure
    // should be 14:02.
    static char ROUTE_TO_TEST[] = "09_2";
    static char STOP_ID[] = "71779";
    result = atb_get_next_departures(1736344840, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736344920, result.resultSet[0]);

    // This is a Saturday at 13:37. The next departure should be 13:47.
    int next = atb_get_next_departure(1736599075, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736599620, next);

    // This is friday the last tram.
    result = atb_get_next_departures(1736548975, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736549220, result.resultSet[0]);

    // Last on Sat.
    result = atb_get_next_departures(1736635375, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736635620, result.resultSet[0]);

    // 12.30 on Sunday.
    result = atb_get_next_departures(1736681400, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736682420, result.resultSet[0]);

    // Last on Sunday.
    result = atb_get_next_departures(1736721000, ROUTE_TO_TEST, STOP_ID);
    TEST_ASSERT_EQUAL(1736722020, result.resultSet[0]);
}

time_t get_time_in_cet_from_string(const char *datetime) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    sscanf(datetime, "%d-%d-%d %d:%d:%d",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_isdst = -1; // Not set by the program

    // Set the timezone to CET
    setenv("TZ", "CET", 1);
    tzset();

    time_t t = mktime(&tm);
    return t;
}

void unix_timestamp_to_hhmm(time_t timestamp, char *buffer, size_t buffer_size) {
    struct tm *tm_info;

    // Set the timezone to CET
    setenv("TZ", "CET", 1);
    tzset();

    tm_info = localtime(&timestamp);
    strftime(buffer, buffer_size, "%H:%M", tm_info);
}

void test_get_next_departures_route_11(void) {
  ResultSet result;

  // Test for stop 74265. This is kongens gate. I tried to take the bus there yesterday (2025-01-30)
  // at around 20:00 and the data was wrong. Let's calculate it. I see there is one leaving 19:48 and
  // in 74265 20 mins later.
  static char ROUTE_TO_TEST[] = "11_1";
  static char STOP_ID[] = "74265";

  time_t timestamp = get_time_in_cet_from_string("2025-01-30 20:00:00");
  result = atb_get_next_departures(timestamp, ROUTE_TO_TEST, STOP_ID);
  char time_str[6];
  unix_timestamp_to_hhmm(result.resultSet[0], time_str, sizeof(time_str));
  TEST_ASSERT_EQUAL_STRING("20:08", time_str);

  time_t timestamp = get_time_in_cet_from_string("2025-02-01 20:00:00");
  result = atb_get_next_departures(timestamp, ROUTE_TO_TEST, STOP_ID);
  unix_timestamp_to_hhmm(result.resultSet[0], time_str, sizeof(time_str));
  TEST_ASSERT_EQUAL_STRING("20:05", time_str);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_next_departures_route_9);
  RUN_TEST(test_get_next_departures_route_11);
  return UNITY_END();
}
