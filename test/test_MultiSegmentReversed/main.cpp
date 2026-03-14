#define NO_GLOBAL_SERIAL

#include <Arduino.h>
#include <unity.h>

static int mapMultiSegmentReversedIndex(int pixel, int totalLeds)
{
    return totalLeds - pixel - 1;
}

void test_multi_segment_reversed_index_mapping()
{
    const int totalLeds = 1025;
    TEST_ASSERT_EQUAL_INT(511, mapMultiSegmentReversedIndex(513, totalLeds));
    TEST_ASSERT_EQUAL_INT(510, mapMultiSegmentReversedIndex(514, totalLeds));
    TEST_ASSERT_EQUAL_INT(0, mapMultiSegmentReversedIndex(1024, totalLeds));
}

void setup()
{
    delay(1000);
    UNITY_BEGIN();
    RUN_TEST(test_multi_segment_reversed_index_mapping);
    UNITY_END();
}

void loop()
{
}
