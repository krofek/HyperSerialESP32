/* fastled_adapter.h
*
*  MIT License
*
*  Copyright (c) 2021-2026 awawa-dev
*
*  https://github.com/awawa-dev/HyperSerialESP32
*/

#pragma once

#include <stdint.h>
#include "led_driver.h"

#define HYPERSERIAL_USE_FASTLED 1

#if !defined(HYPERSERIAL_TESTING)
	#include <FastLED.h>
#endif

#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
	#define LED_DRIVER FastLedClocklessStrip<HS_DATA_PIN, GRB>
#elif defined(SPILED_APA102)
	#define LED_DRIVER FastLedApa102Strip<HS_DATA_PIN, HS_CLOCK_PIN, BGR>
#elif defined(SPILED_WS2801)
	#define LED_DRIVER FastLedWs2801Strip<HS_DATA_PIN, HS_CLOCK_PIN, RGB>
#endif


#if defined(SECOND_SEGMENT_START_INDEX)
	#if defined(NEOPIXEL_RGBW)
		#define LED_DRIVER2 FastLedClocklessStrip<HS_SECOND_SEGMENT_DATA_PIN, GRB>
	#elif defined(NEOPIXEL_RGB)
		#define LED_DRIVER2 FastLedClocklessStrip<HS_SECOND_SEGMENT_DATA_PIN, GRB>
	#elif defined(SPILED_APA102)
		#define LED_DRIVER2 FastLedApa102Strip<HS_SECOND_SEGMENT_DATA_PIN, HS_SECOND_SEGMENT_CLOCK_PIN, BGR>
	#elif defined(SPILED_WS2801)
		#define LED_DRIVER2 FastLedWs2801Strip<HS_SECOND_SEGMENT_DATA_PIN, HS_SECOND_SEGMENT_CLOCK_PIN, RGB>
	#endif
#endif

template<typename TDerived>
#if !defined(HYPERSERIAL_TESTING)
class FastLedStripBase : public ILedDriver
{
	protected:
		int ledCount;
		CRGB* leds;

		inline void setRgbwPixel(uint16_t indexPixel, RgbwColor color)
		{
			leds[indexPixel].setRGB(qadd8(color.R, color.W), qadd8(color.G, color.W), qadd8(color.B, color.W));
		}

	public:
		explicit FastLedStripBase(int count)
		{
			ledCount = count;
			leds = new CRGB[count]();
		}

		~FastLedStripBase() override
		{
			delete[] leds;
		}

		bool CanShow() override
		{
			return true;
		}

		void Show(bool = true) override
		{
			FastLED.show();
		}

		void Begin() override
		{
			static_cast<TDerived*>(this)->BeginImpl();
		}

		void Begin(int, int, int, int) override
		{
			Begin();
		}

		void SetPixelColor(uint16_t indexPixel, RgbColor color) override
		{
			leds[indexPixel].setRGB(color.R, color.G, color.B);
		}

		void SetPixelColor(uint16_t indexPixel, RgbwColor color) override
		{
			setRgbwPixel(indexPixel, color);
		}
};

template<uint8_t DATA_GPIO, uint8_t PIXEL_ORDER_VALUE>
class FastLedClocklessStrip : public FastLedStripBase<FastLedClocklessStrip<DATA_GPIO, PIXEL_ORDER_VALUE>>
{
	public:
		using Base = FastLedStripBase<FastLedClocklessStrip<DATA_GPIO, PIXEL_ORDER_VALUE>>;

		FastLedClocklessStrip(int count, int)
			: Base(count)
		{
		}

		void BeginImpl()
		{
			#if defined(NEOPIXEL_RGBW)
				FastLED.addLeds<SK6812, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
			#else
				FastLED.addLeds<WS2812B, DATA_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
			#endif
		}
};

template<uint8_t DATA_GPIO, uint8_t CLOCK_GPIO, uint8_t PIXEL_ORDER_VALUE>
class FastLedApa102Strip : public FastLedStripBase<FastLedApa102Strip<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>
{
	public:
		using Base = FastLedStripBase<FastLedApa102Strip<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>;

		FastLedApa102Strip(int count)
			: Base(count)
		{
		}

		void BeginImpl()
		{
			FastLED.addLeds<APA102, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
		}
};

template<uint8_t DATA_GPIO, uint8_t CLOCK_GPIO, uint8_t PIXEL_ORDER_VALUE>
class FastLedWs2801Strip : public FastLedStripBase<FastLedWs2801Strip<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>
{
	public:
		using Base = FastLedStripBase<FastLedWs2801Strip<DATA_GPIO, CLOCK_GPIO, PIXEL_ORDER_VALUE>>;

		FastLedWs2801Strip(int count)
			: Base(count)
		{
		}

		void BeginImpl()
		{
			FastLED.addLeds<WS2801, DATA_GPIO, CLOCK_GPIO, static_cast<EOrder>(PIXEL_ORDER_VALUE)>(this->leds, this->ledCount);
		}
};
#endif
