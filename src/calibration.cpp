#include "calibration.h"

#if defined(HS_NEOPIXEL_RGBW) || defined(HYPERSERIAL_TESTING)

Calibration calibration;

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

void Calibration::print()
{
    char output[128];
    snprintf(output, sizeof(output), "RGBW => Gain: %i/255, red: %i, green: %i, blue: %i\r\n", gain, red, green, blue);
    Serial.print(output);
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

#endif
