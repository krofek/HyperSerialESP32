/* led_controller.h
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

#include "config.h"
#include "fastled_adapter.h"
#include "statistics.h"

class LedController
{
  private:
    uint8_t buffer[MAX_BUFFER + 1] = {0};

    int queueCurrent = 0;
    int queueEnd = 0;
    int ledsNumber = 0;
    bool readyToRender = false;

    LED_DRIVER *ledStrip1 = nullptr;
#if defined(SECOND_SEGMENT_START_INDEX)
    LED_DRIVER2 *ledStrip2 = nullptr;
#endif

  public:
    int getLedsNumber();

    bool canRender(bool newFrame);

    LED_DRIVER *getLedStrip1();
#if defined(SECOND_SEGMENT_START_INDEX)
    LED_DRIVER2 *getLedStrip2();
#endif

    void initLedStrip(int count);

    /**
     * @brief Check if there is already prepared frame to display
     *
     * @return true
     * @return false
     */
    bool hasLateFrameToRender();

    void dropLateFrame();

    void renderLeds();

    bool setStripPixel(uint16_t pix, RgbwColor &inputColor);

    bool isAtEndOfQueue();

    int getQueueCurrent();

    int getQueueEnd();

    void setQueueCurrent(int newQueueCurrent);

    void setQueueEnd(int newQueueEnd);

    byte getCurrentInput();

    uint8_t *getBuffer(int index);
};

extern LedController controller;