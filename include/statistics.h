/* stats.h
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

#include <Arduino.h>

#include "calibration.h"
#include <algorithm>

// statistics (stats sent only when there is no communication)
class Statistics
{
    unsigned long startTime = 0;
    uint16_t goodFrames = 0;
    uint16_t showFrames = 0;
    uint16_t totalFrames = 0;
    uint16_t finalGoodFrames = 0;
    uint16_t finalShowFrames = 0;
    uint16_t finalTotalFrames = 0;

  public:
    /**
     * @brief Get the start time of the current period
     *
     * @return unsigned long
     */
    unsigned long getStartTime();

    /**
     * @brief Detected new frame
     *
     */
    void increaseTotal();

    /**
     * @brief The frame is received and shown
     *
     */
    void increaseShow();

    /**
     * @brief The frame is received correctly (not yet displayed)
     *
     */
    void increaseGood();

    /**
     * @brief Get number of correctly received frames
     *
     * @return uint16_t
     */
    uint16_t getGoodFrames();

    /**
     * @brief Period restart, save current statistics ans send them later if there
     * is no incoming communication
     *
     * @param currentTime
     */
    void update(unsigned long currentTime);

    /**
     * @brief Print last saved statistics to the serial port
     *
     * @param curTime
     * @param taskHandle
     */
    void print(unsigned long curTime, TaskHandle_t taskHandle1, TaskHandle_t taskHandle2);

    /**
     * @brief Reset statistics
     *
     */
    void reset(unsigned long currentTime);

    /**
     * @brief Light reset statistics
     *
     * @param curTime The current time in milliseconds
     * @param hasData Indicates if there is data available
     */
    void lightReset(unsigned long curTime, bool hasData);

    /**
     * @brief Handle statistics update based on the current time, delta time, and data availability
     *
     * @param currentTime The current time in milliseconds
     * @param deltaTime The time difference since the last update in milliseconds
     * @param hasData Indicates if there is data available
     */
    void handle(unsigned long currentTime, unsigned long deltaTime, bool hasData);
};

extern Statistics statistics;