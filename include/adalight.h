/* adalight.h
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
#include "statistics.h"
#include "led_controller.h"
#include "framestate.h"

class Adalight {
	public:
		TaskHandle_t processDataHandle = nullptr;
		TaskHandle_t processSerialHandle = nullptr;
		// semaphore to synchronize them
		xSemaphoreHandle i2sXSemaphore;

		static void processDataTaskStatic(void * parameters)
		{
			Adalight *self = static_cast<Adalight*>(parameters);
			self->processDataTask(parameters);
		}

		static void processSerialTaskStatic(void * parameters)
		{
			Adalight *self = static_cast<Adalight*>(parameters);
			self->processSerialTask(parameters);
		}

		/**
		 * @brief separete thread for handling incoming data using cyclic buffer
		 *
		 * @param parameters
		 */
		void processDataTask(void * parameters)
		{
			for(;;)
			{
				xSemaphoreTake(i2sXSemaphore, portMAX_DELAY);
				processData();
			}
		}

		void processSerialTask(void * parameters)
		{
			for(;;)
			{
				if (serialTaskHandler() || base.queueCurrent != base.queueEnd)
					xSemaphoreGive(i2sXSemaphore);
				yield();
			}
		}

		bool serialTaskHandler()
		{
			uint16_t incomingSize = min(Serial.available(), MAX_BUFFER - 1);

			if (incomingSize > 0)
			{
				if (base.queueEnd + incomingSize < MAX_BUFFER)
				{
					Serial.read(&(base.buffer[base.queueEnd]), incomingSize);
					base.queueEnd += incomingSize;
				}
				else
				{
					int left = MAX_BUFFER - base.queueEnd;
					Serial.read(&(base.buffer[base.queueEnd]), left);
					Serial.read(&(base.buffer[0]), incomingSize - left);
					base.queueEnd = incomingSize - left;
				}
			}

		#if defined(LED_POWER_PIN)
			powerControl.update(incomingSize > 0);
		#endif

			return (incomingSize > 0);
		}

		void updateAdalightStatistics(unsigned long currentTime, unsigned long deltaTime, bool hasData)
		{
			if (hasData && deltaTime >= 1000 && deltaTime <= 1025 && statistics.getGoodFrames() > 3)
				statistics.update(currentTime);
			else if (deltaTime > 1025)
				statistics.lightReset(currentTime, hasData);
		}


		void processData()
		{
			// update and print statistics
			unsigned long currentTime = millis();
			unsigned long deltaTime = currentTime - statistics.getStartTime();

			updateAdalightStatistics(currentTime, deltaTime, base.queueCurrent != base.queueEnd);

			if (statistics.getStartTime() + 5000 < millis())
			{
				frameState.setState(AwaProtocol::HEADER_A);
			}

			// render waiting frame if available
			if (base.hasLateFrameToRender())
				base.renderLeds(false);

			// process received data
			while (base.queueCurrent != base.queueEnd)
			{
				byte input = base.buffer[base.queueCurrent++];

				if (base.queueCurrent >= MAX_BUFFER)
				{
					base.queueCurrent = 0;
					yield();
				}

				switch (frameState.getState())
				{
				case AwaProtocol::HEADER_A:
					// assume it's protocol version 1, verify it later
					frameState.setProtocolVersion2(false);
					if (input == 'A')
						frameState.setState(AwaProtocol::HEADER_w);
					break;

				case AwaProtocol::HEADER_w:
					if (input == 'w')
						frameState.setState(AwaProtocol::HEADER_a);
					else
						frameState.setState(AwaProtocol::HEADER_A);
					break;

				case AwaProtocol::HEADER_a:
					// detect protocol version
					if (input == 'a')
						frameState.setState(AwaProtocol::HEADER_HI);
					else if (input == 'A')
					{
						frameState.setState(AwaProtocol::HEADER_HI);
						frameState.setProtocolVersion2(true);
					}
					else
						frameState.setState(AwaProtocol::HEADER_A);
					break;

				case AwaProtocol::HEADER_HI:
					// initialize new frame properties
					statistics.increaseTotal();
					frameState.init(input);
					frameState.setState(AwaProtocol::HEADER_LO);
					break;

				case AwaProtocol::HEADER_LO:
					frameState.computeCRC(input);
					frameState.setState(AwaProtocol::HEADER_CRC);
					break;

				case AwaProtocol::HEADER_CRC:
					// verify CRC and create/update LED driver if neccesery
					if (frameState.getCRC() == input)
					{
						uint16_t ledSize = frameState.getCount() + 1;

						// sanity check
						if (ledSize > 4096)
							frameState.setState(AwaProtocol::HEADER_A);
						else
						{
							if (ledSize != base.getLedsNumber())
								base.initLedStrip(ledSize);

							frameState.setState(AwaProtocol::RED);
						}
					}
					else if (frameState.getCount() ==  0x2aa2 && (input == 0x15 || input == 0x35))
					{
						statistics.print(currentTime, processDataHandle, processSerialHandle);

						if (input == 0x15)
							Serial.println(HELLO_MESSAGE);
						delay(10);

						currentTime = millis();
						statistics.reset(currentTime);
						frameState.setState(AwaProtocol::HEADER_A);
					}
					else
						frameState.setState(AwaProtocol::HEADER_A);
					break;

				case AwaProtocol::RED:
					frameState.color.R = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::GREEN);
					break;

				case AwaProtocol::GREEN:
					frameState.color.G = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::BLUE);
					break;

				case AwaProtocol::BLUE:
					frameState.color.B = input;
					frameState.addFletcher(input);

					#ifdef NEOPIXEL_RGBW
						// calculate RGBW from RGB using provided calibration data
						frameState.rgb2rgbw();
					#endif

					// set pixel, increase the index and check if it was the last LED color to come
					if (base.setStripPixel(frameState.getCurrentLedIndex(), frameState.color))
					{
						frameState.setState(AwaProtocol::RED);
					}
					else
					{
						if (frameState.isProtocolVersion2())
							frameState.setState(AwaProtocol::VERSION2_GAIN);
						else
							frameState.setState(AwaProtocol::FLETCHER1);
					}

					break;

				case AwaProtocol::VERSION2_GAIN:
					frameState.calibration.gain = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::VERSION2_RED);
					break;

				case AwaProtocol::VERSION2_RED:
					frameState.calibration.red = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::VERSION2_GREEN);
					break;

				case AwaProtocol::VERSION2_GREEN:
					frameState.calibration.green = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::VERSION2_BLUE);
					break;

				case AwaProtocol::VERSION2_BLUE:
					frameState.calibration.blue = input;
					frameState.addFletcher(input);

					frameState.setState(AwaProtocol::FLETCHER1);
					break;

				case AwaProtocol::FLETCHER1:
					// initial frame data integrity check
					if (input != frameState.getFletcher1())
						frameState.setState(AwaProtocol::HEADER_A);
					else
						frameState.setState(AwaProtocol::FLETCHER2);
					break;

				case AwaProtocol::FLETCHER2:
					// initial frame data integrity check
					if (input != frameState.getFletcher2())
						frameState.setState(AwaProtocol::HEADER_A);
					else
						frameState.setState(AwaProtocol::FLETCHER_EXT);
					break;

				case AwaProtocol::FLETCHER_EXT:
					// final frame data integrity check
					if (input == frameState.getFletcherExt())
					{
						statistics.increaseGood();

						base.renderLeds(true);

						#ifdef NEOPIXEL_RGBW
							// if received the calibration data, update it now
							if (frameState.isProtocolVersion2())
							{
								frameState.updateIncomingCalibration();
							}
						#endif

						currentTime = millis();
						deltaTime = currentTime - statistics.getStartTime();
						updateAdalightStatistics(currentTime, deltaTime, true);

						yield();
					}

					frameState.setState(AwaProtocol::HEADER_A);
					break;
				}
			}
		}

		void setupMultiCore()
		{
			xTaskCreatePinnedToCore(
				Adalight::processDataTaskStatic,
				"ProcessDataTask",
				4096,
				this,
				1,
				&processDataHandle,
				0);

			xTaskCreatePinnedToCore(
				Adalight::processSerialTaskStatic,
				"ProcessSerialTask",
				4096,
				this,
				1,
				&processSerialHandle,
				1);
		}
};
