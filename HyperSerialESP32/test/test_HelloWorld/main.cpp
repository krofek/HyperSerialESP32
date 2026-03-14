#include <Arduino.h>
#include <unity.h>

void test_HelloWorld() {
    TEST_ASSERT_EQUAL_STRING("Hello, World!", "Hello, World!");
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_HelloWorld);
    UNITY_END();
}

void loop() {
}