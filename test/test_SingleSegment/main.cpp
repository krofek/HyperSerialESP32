#define NO_GLOBAL_SERIAL

#include <Arduino.h>
#include <unity.h>

static int mapSingleSegmentIndex(int pixel)
{
	return pixel;
}

void test_single_segment_index_identity()
{
	TEST_ASSERT_EQUAL_INT(0, mapSingleSegmentIndex(0));
	TEST_ASSERT_EQUAL_INT(42, mapSingleSegmentIndex(42));
	TEST_ASSERT_EQUAL_INT(800, mapSingleSegmentIndex(800));
}

void setup()
{
	delay(1000);
	UNITY_BEGIN();
	RUN_TEST(test_single_segment_index_identity);
	UNITY_END();
}

void loop()
{
}
