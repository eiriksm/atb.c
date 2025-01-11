#include "unity.h"
#include "atb.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_get_next_departures_route_9(void) {
    ResultSet result;

    // Test for stop 71779. This is a Wednesday at 14:00:40. The next departure
    // should be 14:02.
    result = atb_get_next_departures(1736344840, "9", "71779");
    TEST_ASSERT_EQUAL(1736344920, result.resultSet[0]);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_get_next_departures_route_9);
  return UNITY_END();
}
