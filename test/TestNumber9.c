#include "unity.h"
#include "AtB.h"

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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_next_departures_route_9);
  return UNITY_END();
}
