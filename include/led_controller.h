/* led_controller.h
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

#include "freertos/semphr.h"
#include "calibration.h"
#include "statistics.h"
#include "fastled_adapter.h"
#include "config.h"

#if defined(SECOND_SEGMENT_START_INDEX)
	#if !defined(HS_SECOND_SEGMENT_DATA_PIN)
		#error "Please define HS_SECOND_SEGMENT_DATA_PIN for second segment"
	#elif !defined(HS_SECOND_SEGMENT_CLOCK_PIN) && !defined(NEOPIXEL_RGBW) && !defined(NEOPIXEL_RGB)
		#error "Please define HS_SECOND_SEGMENT_CLOCK_PIN and HS_SECOND_SEGMENT_DATA_PIN for second segment"
	#endif
#endif

class Base
{
	int ledsNumber = 0;
	LED_DRIVER* ledStrip1 = nullptr;
#if defined(SECOND_SEGMENT_START_INDEX)
	LED_DRIVER2* ledStrip2 = nullptr;
#endif
	// frame is set and ready to render
	bool readyToRender = false;

	public:
		// static data buffer for the loop
		uint8_t buffer[MAX_BUFFER + 1] = {0};
		volatile int queueCurrent = 0;
		// queue end position
		volatile int queueEnd = 0;

		inline int getLedsNumber()
		{
			return ledsNumber;
		}

		inline LED_DRIVER* getLedStrip1()
		{
			return ledStrip1;
		}
#if defined(SECOND_SEGMENT_START_INDEX)
		inline LED_DRIVER2* getLedStrip2()
		{
			return ledStrip2;
		}
#endif

		void initLedStrip(int count)
		{
			if (ledStrip1 != nullptr)
			{
				delete ledStrip1;
				ledStrip1 = nullptr;
			}

#if defined(SECOND_SEGMENT_START_INDEX)
			if (ledStrip2 != nullptr)
			{
				delete ledStrip2;
				ledStrip2 = nullptr;
			}
#endif

			ledsNumber = count;

			#if defined(SECOND_SEGMENT_START_INDEX)
				if (ledsNumber > SECOND_SEGMENT_START_INDEX)
				{
					#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
						ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX, HS_DATA_PIN);
						ledStrip1->Begin();
						ledStrip2 = new LED_DRIVER2(ledsNumber - SECOND_SEGMENT_START_INDEX, HS_SECOND_SEGMENT_DATA_PIN);
						ledStrip2->Begin();
					#else
						ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX);
						ledStrip1->Begin(HS_CLOCK_PIN, 12, HS_DATA_PIN, 15);
						ledStrip2 = new LED_DRIVER2(ledsNumber - SECOND_SEGMENT_START_INDEX);
						ledStrip2->Begin(HS_SECOND_SEGMENT_CLOCK_PIN, 12, HS_SECOND_SEGMENT_DATA_PIN, 15);
					#endif
				}
			#endif

			if (ledStrip1 == nullptr)
			{
				#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
					ledStrip1 = new LED_DRIVER(ledsNumber, HS_DATA_PIN);
					ledStrip1->Begin();
				#else
					ledStrip1 = new LED_DRIVER(ledsNumber);
					ledStrip1->Begin(HS_CLOCK_PIN, 12, HS_DATA_PIN, 15);
				#endif
			}
		}

		/**
		 * @brief Check if there is already prepared frame to display
		 *
		 * @return true
		 * @return false
		 */
		inline bool hasLateFrameToRender()
		{
			return readyToRender;
		}

		inline void dropLateFrame()
		{
			readyToRender = false;
		}

#if defined(SECOND_SEGMENT_START_INDEX)				
		inline void renderLeds(bool newFrame)
		{
			if (newFrame)
				readyToRender = true;

		
			if (readyToRender &&
				(ledStrip1 != nullptr && ledStrip1->CanShow()) &&
				!(ledStrip2 != nullptr && !ledStrip2->CanShow()))
			{
				statistics.increaseShow();
				readyToRender = false;

				// display segments
				ledStrip1->Show(false);
				#if !defined(HYPERSERIAL_USE_FASTLED) || defined(HYPERSERIAL_TESTING)
					if (ledStrip2 != nullptr)
						ledStrip2->Show(false);
				#endif
			}
		}
#else
		inline void renderLeds(bool newFrame)
		{
			if (newFrame)
				readyToRender = true;

			if (readyToRender && ledStrip1 != nullptr && ledStrip1->CanShow())
			{
				statistics.increaseShow();
				readyToRender = false;
				ledStrip1->Show(false);
			}
		}
#endif

		inline bool setStripPixel(uint16_t pix, ColorDefinition &inputColor)
		{
			if (pix < ledsNumber)
			{
				#if defined(SECOND_SEGMENT_START_INDEX)
					if (pix < SECOND_SEGMENT_START_INDEX)
						ledStrip1->SetPixelColor(pix, inputColor);
					else
					{
						#if defined(SECOND_SEGMENT_REVERSED)
							ledStrip2->SetPixelColor(ledsNumber - pix - 1, inputColor);
						#else
							ledStrip2->SetPixelColor(pix - SECOND_SEGMENT_START_INDEX, inputColor);
						#endif
					}
				#else
					ledStrip1->SetPixelColor(pix, inputColor);
				#endif
			}

			return (pix + 1 < ledsNumber);
		}
} base;

#endif