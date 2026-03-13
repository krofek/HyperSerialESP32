#pragma once

#include <stdint.h>

struct RgbColor
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

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
		virtual bool CanShow() = 0;
		virtual void Show(bool safe = true) = 0;
		virtual void Begin() = 0;
		virtual void Begin(int, int, int, int) = 0;
		virtual void SetPixelColor(uint16_t indexPixel, RgbColor color) = 0;
		virtual void SetPixelColor(uint16_t indexPixel, RgbwColor color) = 0;
};