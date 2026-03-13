#include "adalight.h"

#include "config.h"
#include "statistics.h"
#include "led_controller.h"
#include "framestate.h"

#if defined(LED_POWER_PIN)
#include "powercontrol.h"
#endif

void Adalight::processDataTaskStatic(void * parameters)
{
	Adalight *self = static_cast<Adalight*>(parameters);
	self->processDataTask(parameters);
}

void Adalight::processSerialTaskStatic(void * parameters)
{
	Adalight *self = static_cast<Adalight*>(parameters);
	self->processSerialTask(parameters);
}

void Adalight::processDataTask(void * parameters)
{
	for(;;)
	{
		xSemaphoreTake(i2sXSemaphore, portMAX_DELAY);
		processData();
	}
}

void Adalight::processSerialTask(void * parameters)
{
	for(;;)
	{
		if (serialTaskHandler() || controller.queueCurrent != controller.queueEnd)
			xSemaphoreGive(i2sXSemaphore);
		yield();
	}
}

bool Adalight::serialTaskHandler()
{
	uint16_t incomingSize = min(Serial.available(), MAX_BUFFER - 1);

	if (incomingSize > 0)
	{
		if (controller.queueEnd + incomingSize < MAX_BUFFER)
		{
			Serial.read(&(controller.buffer[controller.queueEnd]), incomingSize);
			controller.queueEnd += incomingSize;
		}
		else
		{
			int left = MAX_BUFFER - controller.queueEnd;
			Serial.read(&(controller.buffer[controller.queueEnd]), left);
			Serial.read(&(controller.buffer[0]), incomingSize - left);
			controller.queueEnd = incomingSize - left;
		}
	}

#if defined(LED_POWER_PIN)
	powerControl.update(incomingSize > 0);
#endif

	return (incomingSize > 0);
}

void Adalight::updateAdalightStatistics(unsigned long currentTime, unsigned long deltaTime, bool hasData)
{
	if (hasData && deltaTime >= 1000 && deltaTime <= 1025 && statistics.getGoodFrames() > 3)
		statistics.update(currentTime);
	else if (deltaTime > 1025)
		statistics.lightReset(currentTime, hasData);
}

void Adalight::processData()
{
	unsigned long currentTime = millis();
	unsigned long deltaTime = currentTime - statistics.getStartTime();

	updateAdalightStatistics(currentTime, deltaTime, controller.queueCurrent != controller.queueEnd);

	if (statistics.getStartTime() + 5000 < millis())
	{
		frameState.setState(AwaProtocol::HEADER_A);
	}

	if (controller.hasLateFrameToRender())
    {
        if(controller.canRender(false)) {
            statistics.increaseShow();
            controller.renderLeds();
        }
    }
        
	while (controller.queueCurrent != controller.queueEnd)
	{
		byte input = controller.buffer[controller.queueCurrent++];

		if (controller.queueCurrent >= MAX_BUFFER)
		{
			controller.queueCurrent = 0;
			yield();
		}

		switch (frameState.getState())
		{
		case AwaProtocol::HEADER_A:
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
			statistics.increaseTotal();
			frameState.init(input);
            controller.dropLateFrame();
			frameState.setState(AwaProtocol::HEADER_LO);
			break;

		case AwaProtocol::HEADER_LO:
			frameState.computeCRC(input);
			frameState.setState(AwaProtocol::HEADER_CRC);
			break;

		case AwaProtocol::HEADER_CRC:
			if (frameState.getCRC() == input)
			{
				uint16_t ledSize = frameState.getCount() + 1;
				if (ledSize > 4096)
					frameState.setState(AwaProtocol::HEADER_A);
				else
				{
					if (ledSize != controller.getLedsNumber())
						controller.initLedStrip(ledSize);
					frameState.setState(AwaProtocol::RED);
				}
			}
			else if (frameState.getCount() == 0x2aa2 && (input == 0x15 || input == 0x35))
			{
				statistics.print(currentTime, processDataHandle, processSerialHandle);
                #if defined(NEOPIXEL_RGBW)
                    calibration.print();
                #endif

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

			if (controller.setStripPixel(frameState.getCurrentLedIndex(), frameState.color))
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
			if (input != frameState.getFletcher1())
				frameState.setState(AwaProtocol::HEADER_A);
			else
				frameState.setState(AwaProtocol::FLETCHER2);
			break;

		case AwaProtocol::FLETCHER2:
			if (input != frameState.getFletcher2())
				frameState.setState(AwaProtocol::HEADER_A);
			else
				frameState.setState(AwaProtocol::FLETCHER_EXT);
			break;

		case AwaProtocol::FLETCHER_EXT:
			if (input == frameState.getFletcherExt())
			{
				statistics.increaseGood();

                if(controller.canRender(true)) {
                    statistics.increaseShow();
				    controller.renderLeds();
                }
                

#if defined(NEOPIXEL_RGBW)
				if (frameState.isProtocolVersion2())
		            calibration.setParamsAndPrepare(frameState.calibration.gain, frameState.calibration.red, frameState.calibration.green, frameState.calibration.blue);
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

void Adalight::setupMultiCore()
{
	xTaskCreatePinnedToCore(
		processDataTaskStatic,
		"ProcessDataTask",
		4096,
		this,
		1,
		&processDataHandle,
		0);

	xTaskCreatePinnedToCore(
		processSerialTaskStatic,
		"ProcessSerialTask",
		4096,
		this,
		1,
		&processSerialHandle,
		1);
}
