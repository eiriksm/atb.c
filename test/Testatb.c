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

    // Test for stop 2
    result = atb_get_next_departures_route_9(635, 2);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(705, result.departure_times[0]);
    TEST_ASSERT_EQUAL(735, result.departure_times[1]);
    TEST_ASSERT_EQUAL(805, result.departure_times[2]);
    TEST_ASSERT_EQUAL(0, result.departure_times[3]);

    // Test for stop 3
    result = atb_get_next_departures_route_9(640, 3);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(710, result.departure_times[0]);
    TEST_ASSERT_EQUAL(740, result.departure_times[1]);
    TEST_ASSERT_EQUAL(810, result.departure_times[2]);
    TEST_ASSERT_EQUAL(0, result.departure_times[3]);

    // Test for stop 4
    result = atb_get_next_departures_route_9(645, 4);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(715, result.departure_times[0]);
    TEST_ASSERT_EQUAL(745, result.departure_times[1]);
    TEST_ASSERT_EQUAL(815, result.departure_times[2]);
    TEST_ASSERT_EQUAL(0, result.departure_times[3]);

    // Test for stop 5
    result = atb_get_next_departures_route_9(650, 5);
    TEST_ASSERT_EQUAL(4, result.count);
    TEST_ASSERT_EQUAL(720, result.departure_times[0]);
    TEST_ASSERT_EQUAL(750, result.departure_times[1]);
    TEST_ASSERT_EQUAL(820, result.departure_times[2]);
    TEST_ASSERT_EQUAL(0, result.departure_times[3]);
}

void test_get_next_departure(void) {
    int result;

    // Test for route 9, stop 1
    result = atb_get_next_departure(605, '9', 1);
    TEST_ASSERT_EQUAL(630, result);

    // Test for route 9, stop 2
    result = atb_get_next_departure(635, '9', 2);
    TEST_ASSERT_EQUAL(705, result);

    // Test for route 9, stop 3
    result = atb_get_next_departure(640, '9', 3);
    TEST_ASSERT_EQUAL(710, result);

    // Test for route 9, stop 4
    result = atb_get_next_departure(645, '9', 4);
    TEST_ASSERT_EQUAL(715, result);

    // Test for route 9, stop 5
    result = atb_get_next_departure(650, '9', 5);
    TEST_ASSERT_EQUAL(720, result);

    // Test for no next departure
    result = atb_get_next_departure(820, '9', 5);
    TEST_ASSERT_EQUAL(-1, result);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_next_departures_route_9);
  RUN_TEST(test_get_next_departure);
  return UNITY_END();
}
