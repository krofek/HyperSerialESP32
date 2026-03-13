/* framestate.h
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

#include <Arduino.h>
#include "calibration.h"
#include "led_controller.h"

/**
 * @brief my AWA frame protocol definition
 *
 */
enum class AwaProtocol
{
	HEADER_A,
	HEADER_w,
	HEADER_a,
	HEADER_HI,
	HEADER_LO,
	HEADER_CRC,
	VERSION2_GAIN,
	VERSION2_RED,
	VERSION2_GREEN,
	VERSION2_BLUE,
	RED,
	GREEN,
	BLUE,
	FLETCHER1,
	FLETCHER2,
	FLETCHER_EXT
};

/**
 * @brief Contains current state of the incoming frame
 *
 */
class FrameState
{
	volatile AwaProtocol state = AwaProtocol::HEADER_A;
	bool protocolVersion2 = false;
	uint8_t CRC = 0;
	uint16_t count = 0;
	uint16_t currentLed = 0;
	uint16_t fletcher1 = 0;
	uint16_t fletcher2 = 0;
	uint16_t fletcherExt = 0;
	uint8_t position = 0;

	public:
		ColorDefinition color;

		/**
		 * @brief Reset statistics for new frame
		 *
		 * @param input
		 */
		void init(byte input);

		/**
		 * @brief get computed CRC
		 *
		 * @return uint8_t
		 */
		uint8_t getCRC();

		/**
		 * @brief Get the color count reported by the frame
		 *
		 * @return uint16_t
		 */
		uint16_t getCount();

		/**
		 * @brief Get the Fletcher1 total sum
		 *
		 * @return uint16_t
		 */
		uint16_t getFletcher1();

		/**
		 * @brief Get the Fletcher2 total sum
		 *
		 * @return uint16_t
		 */
		uint16_t getFletcher2();

		/**
		 * @brief Get the FletcherExt total sum
		 *
		 * @return uint16_t
		 */
		uint16_t getFletcherExt();

		/**
		 * @brief Get and increase the current Led index
		 *
		 * @return uint16_t
		 */
		uint16_t getCurrentLedIndex();

		/**
		 * @brief Set if frame protocol version 2 (contains calibration data)
		 *
		 * @param newVer
		 */
		void setProtocolVersion2(bool newVer);

		/**
		 * @brief Verify if frame protocol version 2 (contains calibration data)
		 *
		 * @return true
		 * @return false
		 */
		bool isProtocolVersion2();

		/**
		 * @brief  Set new AWA frame state
		 *
		 * @param newState
		 */
		void setState(AwaProtocol newState);

		/**
		 * @brief Get current AWA frame state
		 *
		 * @return AwaProtocol
		 */
		AwaProtocol getState();

		/**
		 * @brief Update CRC based on current and previuos input
		 *
		 * @param input
		 */
		void computeCRC(byte input);

		/**
		 * @brief Update Fletcher checksumn for incoming input
		 *
		 * @param input
		 */
		void addFletcher(byte input);

		/**
		 * @brief Check if the calibration data was updated and calculate new one
		 *
		 */
		void updateIncomingCalibration();


		#ifdef NEOPIXEL_RGBW
			/**
			* @brief Compute && correct the white channel
			*
			*/
			void rgb2rgbw();
		#endif

		/**
		 * @brief Incoming calibration data
		 *
		 */
		struct
		{
			uint8_t gain = 0;
			uint8_t red = 0;
			uint8_t green = 0;
			uint8_t blue = 0;
		} calibration;

};

extern FrameState frameState;
