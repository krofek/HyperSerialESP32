#include "statistics.h"

#include <algorithm>
#include "calibration.h"

Statistics statistics;

unsigned long Statistics::getStartTime()
{
	return startTime;
}

void Statistics::increaseTotal()
{
	totalFrames++;
}

void Statistics::increaseShow()
{
	showFrames++;
}

void Statistics::increaseGood()
{
	goodFrames++;
}

uint16_t Statistics::getGoodFrames()
{
	return goodFrames;
}

void Statistics::update(unsigned long currentTime)
{
	if (totalFrames > 0)
	{
		finalShowFrames = showFrames;
		finalGoodFrames = std::min(goodFrames, totalFrames);
		finalTotalFrames = totalFrames;
	}

	startTime = currentTime;
	goodFrames = 0;
	totalFrames = 0;
	showFrames = 0;
}

void Statistics::print(unsigned long curTime, TaskHandle_t taskHandle1, TaskHandle_t taskHandle2)
{
	char output[128];

	startTime = curTime;
	goodFrames = 0;
	totalFrames = 0;
	showFrames = 0;

	snprintf(output, sizeof(output), "HyperHDR frames: %u (FPS), receiv.: %u, good: %u, incompl.: %u, mem1: %i, mem2: %i, heap: %i\r\n",
			finalShowFrames, finalTotalFrames, finalGoodFrames, (finalTotalFrames - finalGoodFrames),
			(taskHandle1 != nullptr) ? uxTaskGetStackHighWaterMark(taskHandle1) : 0,
			(taskHandle2 != nullptr) ? uxTaskGetStackHighWaterMark(taskHandle2) : 0,
			ESP.getFreeHeap());
	Serial.print(output);

#if defined(NEOPIXEL_RGBW)
	calibrationConfig.printCalibration();
#endif
}

void Statistics::reset(unsigned long currentTime)
{
	startTime = currentTime;

	finalShowFrames = 0;
	finalGoodFrames = 0;
	finalTotalFrames = 0;

	goodFrames = 0;
	totalFrames = 0;
	showFrames = 0;
}

void Statistics::lightReset(unsigned long curTime, bool hasData)
{
	if (hasData)
		startTime = curTime;

	goodFrames = 0;
	totalFrames = 0;
	showFrames = 0;
}
