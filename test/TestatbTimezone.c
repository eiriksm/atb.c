#include "unity.h"
#include "atb.h"
#include <stdlib.h>
#include <time.h>

void setUp(void) {
  setenv("TZ", "UTC", 1);
  tzset();
}

void tearDown(void) {
}

void test_get_departures_with_timestamp_timezone_and_route(void) {
    ResultSet result;
    // Right. First Let's pass in the timestamp generated from the time 09:00 in
    // New York.
    setenv("TZ", "America/New_York", 1);
    tzset();
    // Create a timestamp for the time 09:00 on saturday Jan 18th 2025.
    int timestamp;
    timestamp = 1737208800;
    result = atb_get_next_departures(timestamp, "09_2", "71779");
    TEST_ASSERT_EQUAL_INT(1737209820, result.resultSet[0]);
    // Reset the timezone.
    setenv("TZ", "UTC", 1);
    tzset();
    // Create a timestamp for the time 09:30 on saturday Jan 18th 2025.
    timestamp = 1737189000;
    result = atb_get_next_departures(timestamp, "09_2", "71779");
    TEST_ASSERT_EQUAL_INT(1737190020, result.resultSet[0]);
    // Now do the same, only in the timezone of Berlin.
    setenv("TZ", "Europe/Berlin", 1);
    tzset();
    // Same time, 09:30.
    timestamp = 1737189000;
    result = atb_get_next_departures(timestamp, "09_2", "71779");
    TEST_ASSERT_EQUAL_INT(1737189120, result.resultSet[0]);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_departures_with_timestamp_timezone_and_route);
  return UNITY_END();
}
