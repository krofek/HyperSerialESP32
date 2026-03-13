#include "led_controller.h"

#include "statistics.h"

LedController controller;

int LedController::getLedsNumber()
{
	return ledsNumber;
}

LED_DRIVER* LedController::getLedStrip1()
{
	return ledStrip1;
}

#if defined(SECOND_SEGMENT_START_INDEX)
LED_DRIVER2* LedController::getLedStrip2()
{
	return ledStrip2;
}
#endif

void LedController::initLedStrip(int count)
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

bool LedController::hasLateFrameToRender()
{
	return readyToRender;
}

void LedController::dropLateFrame()
{
	readyToRender = false;
}

bool LedController::canRender(bool newFrame)
{  

#if defined(SECOND_SEGMENT_START_INDEX)
 return newFrame &&
		(ledStrip1 != nullptr && ledStrip1->CanShow()) &&
		!(ledStrip2 != nullptr && !ledStrip2->CanShow());
#else
	return newFrame && ledStrip1 != nullptr && ledStrip1->CanShow();
#endif

}

void LedController::renderLeds()
{
#if defined(SECOND_SEGMENT_START_INDEX)
    readyToRender = false;

    ledStrip1->Show(false);
    if (ledStrip2 != nullptr)
        ledStrip2->Show(false);
#else
	readyToRender = false;
	ledStrip1->Show(false);
#endif
}

bool LedController::setStripPixel(uint16_t pix, ColorDefinition &inputColor)
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
