/* calibration.h
*
*  MIT License
*
*  Copyright (c) 2021-2026 awawa-dev
*
*  https://github.com/awawa-dev/HyperSerialESP32
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
all
*  copies or substantial portions of the Software.

*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*  SOFTWARE.
 */

#pragma once

#if defined(NEOPIXEL_RGBW)

#include "config.h"
#include "fastled_adapter.h"
#include <algorithm>
#include <stdint.h>

struct ChannelCorrection
{
    uint8_t white[256];
    uint8_t red[256];
    uint8_t green[256];
    uint8_t blue[256];
};

class Calibration
{
  private:
    uint8_t gain = 0xFF;
    uint8_t red = 0xA0;
    uint8_t green = 0xA0;
    uint8_t blue = 0xA0;

  public:
    ChannelCorrection channelCorrection;

    Calibration();

    /**
     * @brief Compare base calibration settings
     *
     */
    bool compareSettings(uint8_t _gain, uint8_t _red, uint8_t _green, uint8_t _blue);

    /**
     * @brief Prepare calibration
     *
     */
    void prepare();

    /**
     * @brief print RGBW calibration parameters when no data is received
     *
     */
    void print();

    /**
     * @brief Set gain value
     *
     * @param newGain
     */
    void setGain(uint8_t newGain);

    /**
     * @brief Set red channel value
     *
     * @param newRed
     */
    void setRed(uint8_t newRed);

    /**
     * @brief Set green channel value
     *
     * @param newGreen
     */
    void setGreen(uint8_t newGreen);

    /**
     * @brief Set blue channel value
     *
     * @param newBlue
     */
    void setBlue(uint8_t newBlue);

    /**
     * @brief Set all calibration parameters and prepare correction tables
     *
     * @param newGain
     * @param newRed
     * @param newGreen
     * @param newBlue
     */
    void setParamsAndPrepare(uint8_t newGain, uint8_t newRed, uint8_t newGreen, uint8_t newBlue);
};

extern Calibration calibration;

#endif
