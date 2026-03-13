#include "calibration.h"

#if defined(NEOPIXEL_RGBW) || defined(HYPERSERIAL_TESTING)

#include <algorithm>

ChannelCorrection channelCorrection;
Calibration calibrationConfig;

void Calibration::prepare()
{
	for (uint32_t i = 0; i < 256; i++)
	{
		channelCorrection.white[i] = (uint8_t)std::min(ROUND_DIVIDE(gain * i, 0xFF), (uint32_t)0xFF);
		channelCorrection.red[i] = (uint8_t)std::min(ROUND_DIVIDE(red * i, 0xFF), (uint32_t)0xFF);
		channelCorrection.green[i] = (uint8_t)std::min(ROUND_DIVIDE(green * i, 0xFF), (uint32_t)0xFF);
		channelCorrection.blue[i] = (uint8_t)std::min(ROUND_DIVIDE(blue * i, 0xFF), (uint32_t)0xFF);
	}
}

Calibration::Calibration()
{
	prepare();
}

bool Calibration::compareSettings(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue)
{
	return _gain == gain && _red == red && _green == green && _blue == blue;
}

void Calibration::setParamsAndPrepare(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue)
{
	if (gain != _gain || red != _red || green != _green || blue != _blue)
	{
		gain = _gain;
		red = _red;
		green = _green;
		blue = _blue;
		prepare();
	}
}

void Calibration::print()
{
	char output[128];
	snprintf(output, sizeof(output), "RGBW => Gain: %i/255, red: %i, green: %i, blue: %i\r\n", gain, red, green, blue);
	Serial.print(output);
}

#endif
