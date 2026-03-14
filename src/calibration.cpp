#include "calibration.h"

#if defined(HS_NEOPIXEL_RGBW) || defined(HYPERSERIAL_TESTING)

Calibration calibration;

void Calibration::prepare()
{
    for (uint32_t i = 0; i < 256; i++)
    {
        channelCorrection.white[i] = CORRECT_CHANNEL(gain * i);
        channelCorrection.red[i] = CORRECT_CHANNEL(red * i);
        channelCorrection.green[i] = CORRECT_CHANNEL(green * i);
        channelCorrection.blue[i] = CORRECT_CHANNEL(blue * i);
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

void Calibration::print()
{
    Serial.printf("RGBW => Gain: %i/255, red: %i, green: %i, blue: %i\r\n", gain, red, green, blue);
}

void Calibration::setGain(uint8_t newGain)
{
    gain = newGain;
}

void Calibration::setRed(uint8_t newRed)
{
    red = newRed;
}

void Calibration::setGreen(uint8_t newGreen)
{
    green = newGreen;
}

void Calibration::setBlue(uint8_t newBlue)
{
    blue = newBlue;
}

void Calibration::setParamsAndPrepare(uint8_t newGain, uint8_t newRed, uint8_t newGreen, uint8_t newBlue)
{
    gain = newGain;
    red = newRed;
    green = newGreen;
    blue = newBlue;
    prepare();
}

void Calibration::rgb2rgbw(RgbwColor &color)
{
    color.W =
        min(channelCorrection.red[color.R], min(channelCorrection.green[color.G], channelCorrection.blue[color.B]));
    color.R -= channelCorrection.red[color.W];
    color.G -= channelCorrection.green[color.W];
    color.B -= channelCorrection.blue[color.W];
    color.W = channelCorrection.white[color.W];
}

#endif
