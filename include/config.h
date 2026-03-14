#pragma once

#include <algorithm>

/**
 * @file config.h
 * @brief Global configuration and helper macros
 */

// buffer size for incoming serial data, should be enough for 3013 leds with RGB and 1 byte for the protocol
#define MAX_BUFFER (3013 * 3 + 1)

#define HELLO_MESSAGE "\r\nWelcome!\r\nAwa driver 9."

// timeout after which the leds will be turned off if no reset is applied
#define LED_POWER_CONTROL_OFF_PERIOD 5000

// macro for rounding and correcting channel values based on gain and calibration
#define CORRECT_CHANNEL(number) (uint8_t)std::min((((number) + 0xFF / 2) / 0xFF), (uint32_t)0xFF)