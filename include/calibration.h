/* calibration.h
*
*  MIT License
*
*  Copyright (c) 2021-2026 awawa-dev
*
*  https://github.com/awawa-dev/HyperSerialESP32
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in all
*  copies or substantial portions of the Software.

*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
 */

#pragma once

#include "fastled_adapter.h"
#include <stdint.h>

#ifdef NEOPIXEL_RGBW
	typedef RgbwColor ColorDefinition;
#else
	typedef RgbColor ColorDefinition;
#endif


#if defined(NEOPIXEL_RGBW) || defined(HYPERSERIAL_TESTING)

struct ChannelCorrection
{
	uint8_t white[256];
	uint8_t red[256];
	uint8_t green[256];
	uint8_t blue[256];
};

extern ChannelCorrection channelCorrection;

class CalibrationConfig
{
	// calibration parameters
	uint8_t gain = 0xFF;
	uint8_t red = 0xA0;
	uint8_t green = 0xA0;
	uint8_t blue = 0xA0;

	void prepareCalibration();

	public:
		CalibrationConfig();

		/**
		 * @brief Compare base calibration settings
		 *
		 */
		bool compareCalibrationSettings(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue);

		/**
		 * @brief Set the parameters that define RGB to RGBW transformation
		 *
		 * @param _gain
		 * @param _red
		 * @param _green
		 * @param _blue
		 */
		void setParamsAndPrepareCalibration(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue);

		/**
		 * @brief print RGBW calibration parameters when no data is received
		 *
		 */
		void printCalibration();
};

extern CalibrationConfig calibrationConfig;
#endif

