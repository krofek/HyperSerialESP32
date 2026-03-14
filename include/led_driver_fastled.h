/* fastled_adapter.h
 *
 *  MIT License
 *
 *  Copyright (c) 2021-2026 awawa-dev
 *
 *  https://github.com/awawa-dev/HyperSerialESP32
 */

#pragma once

#include "led_driver.h"
#include <FastLED.h>
#include <stdint.h>

#if HS_LED_TYPE == HS_NEOPIXEL_RGBW || HS_LED_TYPE == HS_NEOPIXEL_RGB
#define LED_DRIVER ClocklessStripFastLed<HS_DATA_PIN, GRB>
#if defined(HS_SECOND_SEGMENT_START_INDEX)
#define LED_DRIVER2 ClocklessStripFastLed<HS_SECOND_SEGMENT_DATA_PIN, GRB>
#endif
#elif HS_LED_TYPE == HS_SPI_LED_APA102 || HS_LED_TYPE == HS_SPI_LED_WS2801
#define LED_DRIVER SPIStripFastLed<HS_DATA_PIN, HS_CLOCK_PIN, BGR>
#if defined(HS_SECOND_SEGMENT_START_INDEX)
#define LED_DRIVER2 SPIStripFastLed<HS_SECOND_SEGMENT_DATA_PIN, HS_SECOND_SEGMENT_CLOCK_PIN, BGR>
#endif
#endif

template <typename TDerived> class LedDriverFastLed : public ILedDriver
{
  protected:
    int ledCount;
    CRGB *leds;

  public:
    explicit LedDriverFastLed(int count)
    {
        ledCount = count;
        leds = new CRGB[count]();
        begin();
    }

    ~LedDriverFastLed() override
    {
        delete[] leds;
    }

    bool canShow() override
    {
        return true;
    }

    void show() override
    {
        FastLED.show();
    }

    void begin() override
    {
        static_cast<TDerived *>(this)->beginImpl();
    }

    void setPixelColor(uint16_t index, RgbwColor color) override
    {
        leds[index].setRGB(color.R, color.G, color.B);
    }
};

#if HS_LED_TYPE == HS_NEOPIXEL_RGBW || HS_LED_TYPE == HS_NEOPIXEL_RGB
template <uint8_t DATA_GPIO, uint8_t PIXEL_ORDER_VALUE>
class ClocklessStripFastLed : public LedDriverFastLed<ClocklessStripFastLed<DATA_GPIO, PIXEL_ORDER_VALUE>>
{
  public:
    using Base = LedDriverFastLed<ClocklessStripFastLed<DATA_GPIO, PIXEL_ORDER_VALUE>>;

    ClocklessStripFastLed(int count) : Base(count)
    {
    }

    void beginImpl()
    {
#if HS_LED_TYPE == HS_NEOPIXEL_RGBW
        FastLED.addLeds<SK6812, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount)
            .setRgbw(RgbwDefault());
#else
        FastLED.addLeds<WS2812B, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
#endif
    }
};
#endif

#if HS_LED_TYPE == HS_SPI_LED_APA102 || HS_LED_TYPE == HS_SPI_LED_WS2801
template <uint8_t DATA_GPIO, uint8_t CLOCK_GPIO, uint8_t PIXEL_ORDER_VALUE>
class SPIStripFastLed : public LedDriverFastLed<SPIStripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>
{
  public:
    using Base = LedDriverFastLed<SPIStripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>;

    SPIStripFastLed(int count) : Base(count)
    {
    }

    void beginImpl()
    {
#if HS_LED_TYPE == HS_SPI_LED_APA102
        FastLED.addLeds<APA102, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds,
                                                                                               this->ledCount);
#elif HS_LED_TYPE == HS_SPI_LED_WS2801
        FastLED.addLeds<WS2801, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds,
                                                                                               this->ledCount);
#endif
    }
};
#endif