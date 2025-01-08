#include "unity.h"
#include "atb.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_get_next_departures_route_9(void) {
    DepartureTimes result;

    // Test for stop 1
    result = atb_get_next_departures_route_9(605, 1);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(630, result.departure_times[0]);
    TEST_ASSERT_EQUAL(700, result.departure_times[1]);
    TEST_ASSERT_EQUAL(730, result.departure_times[2]);
    TEST_ASSERT_EQUAL(800, result.departure_times[3]);
}

void test_get_next_departure(void) {
    int result;

    // Test for route 9, stop 1
    result = atb_get_next_departure(605, '9', 1);
    TEST_ASSERT_EQUAL(630, result);

    // Test for no next departure
    result = atb_get_next_departure(820, '9', 1);
    TEST_ASSERT_EQUAL(-1, result);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_next_departures_route_9);
  RUN_TEST(test_get_next_departure);
  return UNITY_END();
}
