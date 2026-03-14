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

#if defined(HS_NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
#define LED_DRIVER ClocklessStripFastLed<HS_DATA_PIN, GRB>
#elif defined(SPILED_APA102)
#define LED_DRIVER Apa1002StripFastLed<HS_DATA_PIN, HS_CLOCK_PIN, BGR>
#elif defined(SPILED_WS2801)
#define LED_DRIVER Ws2801StripFastLed<HS_DATA_PIN, HS_CLOCK_PIN, RGB>
#endif

#if defined(SECOND_SEGMENT_START_INDEX)
#if defined(HS_NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
#define LED_DRIVER2 ClocklessStripFastLed<HS_SECOND_SEGMENT_DATA_PIN, GRB>
#elif defined(SPILED_APA102)
#define LED_DRIVER2 Apa1002StripFastLed<HS_SECOND_SEGMENT_DATA_PIN, HS_SECOND_SEGMENT_CLOCK_PIN, BGR>
#elif defined(SPILED_WS2801)
#define LED_DRIVER2 Ws2801StripFastLed<HS_SECOND_SEGMENT_DATA_PIN, HS_SECOND_SEGMENT_CLOCK_PIN, RGB>
#endif
#endif

template <typename TDerived> class LedAdapterFastLed : public ILedDriver
{
  protected:
    int ledCount;
    CRGB *leds;

  public:
    explicit LedAdapterFastLed(int count)
    {
        ledCount = count;
        leds = new CRGB[count]();
        begin();
    }

    ~LedAdapterFastLed() override
    {
        delete[] leds;
    }

    bool canShow() override
    {
        return true;
    }

    void show(bool = true) override
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

template <uint8_t DATA_GPIO, uint8_t PIXEL_ORDER_VALUE>
class ClocklessStripFastLed : public LedAdapterFastLed<ClocklessStripFastLed<DATA_GPIO, PIXEL_ORDER_VALUE>>
{
  public:
    using Base = LedAdapterFastLed<ClocklessStripFastLed<DATA_GPIO, PIXEL_ORDER_VALUE>>;

    ClocklessStripFastLed(int count) : Base(count)
    {
    }

    void beginImpl()
    {
#if defined(HS_NEOPIXEL_RGBW)
        FastLED.addLeds<SK6812, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount)
            .setRgbw(RgbwDefault());
#else
        FastLED.addLeds<WS2812B, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
#endif
    }
};

template <uint8_t DATA_GPIO, uint8_t CLOCK_GPIO, uint8_t PIXEL_ORDER_VALUE>
class Apa1002StripFastLed : public LedAdapterFastLed<Apa1002StripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>
{
  public:
    using Base = LedAdapterFastLed<Apa1002StripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>;

    Apa1002StripFastLed(int count) : Base(count)
    {
    }

    void beginImpl()
    {
        FastLED.addLeds<APA102, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds,
                                                                                               this->ledCount);
    }
};

template <uint8_t DATA_GPIO, uint8_t CLOCK_GPIO, uint8_t PIXEL_ORDER_VALUE>
class Ws2801StripFastLed : public LedAdapterFastLed<Ws2801StripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>
{
  public:
    using Base = LedAdapterFastLed<Ws2801StripFastLed<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>;

    Ws2801StripFastLed(int count) : Base(count)
    {
    }

    void beginImpl()
    {
        FastLED.addLeds<WS2801, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds,
                                                                                               this->ledCount);
    }
};