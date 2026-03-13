#include "calibration.h"

#if defined(NEOPIXEL_RGBW) || defined(HYPERSERIAL_TESTING)

#include <algorithm>

#define ROUND_DIVIDE(numer, denom) (((numer) + (denom) / 2) / (denom))

ChannelCorrection channelCorrection;
CalibrationConfig calibrationConfig;

void CalibrationConfig::prepareCalibration()
{
	for (uint32_t i = 0; i < 256; i++)
	{
		uint32_t _gain = gain * i;
		uint32_t _red = red * i;
		uint32_t _green = green * i;
		uint32_t _blue = blue * i;

		channelCorrection.white[i] = (uint8_t)std::min(ROUND_DIVIDE(_gain, 0xFF), (uint32_t)0xFF);
		channelCorrection.red[i] = (uint8_t)std::min(ROUND_DIVIDE(_red, 0xFF), (uint32_t)0xFF);
		channelCorrection.green[i] = (uint8_t)std::min(ROUND_DIVIDE(_green, 0xFF), (uint32_t)0xFF);
		channelCorrection.blue[i] = (uint8_t)std::min(ROUND_DIVIDE(_blue, 0xFF), (uint32_t)0xFF);
	}
}

CalibrationConfig::CalibrationConfig()
{
	prepareCalibration();
}

bool CalibrationConfig::compareCalibrationSettings(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue)
{
	return _gain == gain && _red == red && _green == green && _blue == blue;
}

void CalibrationConfig::setParamsAndPrepareCalibration(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue)
{
	if (gain != _gain || red != _red || green != _green || blue != _blue)
	{
		gain = _gain;
		red = _red;
		green = _green;
		blue = _blue;
		prepareCalibration();
	}
}

void CalibrationConfig::printCalibration()
{
	char output[128];
	snprintf(output, sizeof(output), "RGBW => Gain: %i/255, red: %i, green: %i, blue: %i\r\n", gain, red, green, blue);
	Serial.print(output);
}

#endif
