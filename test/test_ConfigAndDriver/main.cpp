#define NO_GLOBAL_SERIAL

#include <Arduino.h>
#include <unity.h>

#include "config.h"
#include "led_driver.h"

void test_max_buffer_constant()
{
	TEST_ASSERT_EQUAL_UINT32(9040, MAX_BUFFER);
}

void test_hello_message_content()
{
	TEST_ASSERT_EQUAL_STRING("\r\nWelcome!\r\nAwa driver 9.", HELLO_MESSAGE);
}

void test_led_color_struct_sizes()
{
	TEST_ASSERT_EQUAL_UINT32(3, sizeof(RgbColor));
	TEST_ASSERT_EQUAL_UINT32(4, sizeof(RgbwColor));
}

void setup()
{
	delay(1000);
	UNITY_BEGIN();
	RUN_TEST(test_max_buffer_constant);
	RUN_TEST(test_hello_message_content);
	RUN_TEST(test_led_color_struct_sizes);
	UNITY_END();
}

void loop()
{
}
