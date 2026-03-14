#pragma once

#include <stdint.h>

struct RgbwColor
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;
};

class ILedDriver
{
  public:
    virtual ~ILedDriver() = default;
    virtual bool canShow() = 0;
    virtual void show(bool safe = true) = 0;
    virtual void begin() = 0;
    virtual void setPixelColor(uint16_t index, RgbwColor color) = 0;
};