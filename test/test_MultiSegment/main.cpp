#define NO_GLOBAL_SERIAL

#include <Arduino.h>
#include <unity.h>

static int mapMultiSegmentIndex(int pixel, int secondSegmentStart, bool isSecondSegment)
{
	if (!isSecondSegment)
	{
		return pixel;
	}
	return pixel - secondSegmentStart;
}

void test_multi_segment_first_segment_index()
{
	TEST_ASSERT_EQUAL_INT(0, mapMultiSegmentIndex(0, 513, false));
	TEST_ASSERT_EQUAL_INT(512, mapMultiSegmentIndex(512, 513, false));
}

void test_multi_segment_second_segment_index()
{
	TEST_ASSERT_EQUAL_INT(0, mapMultiSegmentIndex(513, 513, true));
	TEST_ASSERT_EQUAL_INT(17, mapMultiSegmentIndex(530, 513, true));
}

void setup()
{
	delay(1000);
	UNITY_BEGIN();
	RUN_TEST(test_multi_segment_first_segment_index);
	RUN_TEST(test_multi_segment_second_segment_index);
	UNITY_END();
}

void loop()
{
}
