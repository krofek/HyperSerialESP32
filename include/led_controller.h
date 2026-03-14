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
#include "led_driver_fastled.h"

class LedController
{
  private:
    uint8_t buffer[MAX_BUFFER + 1] = {0};

    int queueCurrent = 0;
    int queueEnd = 0;
    int ledsNumber = 0;
    bool readyToRender = false;

    ILedDriver *ledStrip1 = nullptr;
#if defined(HS_SECOND_SEGMENT_START_INDEX)
    ILedDriver *ledStrip2 = nullptr;
#endif

  public:
    int getLedsNumber();

    bool canRender(bool newFrame);

    ILedDriver *getLedStrip1();
#if defined(HS_SECOND_SEGMENT_START_INDEX)
    ILedDriver *getLedStrip2();
#endif

    void initLedStrip(int count);

    /**
     * @brief Check if there is already prepared frame to display
     *
     * @return true
     * @return false
     */
    bool hasLateFrameToRender();

    /**
     * @brief Drop the frame that is waiting to be rendered, if there is one
     *
     */
    void dropLateFrame();

    /**
     * @brief Render the prepared frame to the LED strip
     *
     */
    void renderLeds();

    /**
     * @brief Set the color of a specific pixel in the LED strip
     *
     * @param pixel The index of the pixel to set
     * @param color The color to set the pixel to
     * @return true if the pixel was set successfully, false otherwise
     */
    bool setStripPixel(uint16_t pixel, RgbwColor &color);

    /**
     * @brief Check if the current position in the queue is at the end
     *
     * @return true if at the end of the queue, false otherwise
     */
    bool isAtEndOfQueue();

    /**
     * @brief Get the current position in the queue
     *
     * @return The current position in the queue
     */
    int getQueueCurrent();

    /**
     * @brief Get the end position of the queue
     *
     * @return The end position of the queue
     */
    int getQueueEnd();

    /**
     * @brief Set the current position in the queue
     *
     * @param newQueueCurrent The new current position in the queue
     */
    void setQueueCurrent(int newQueueCurrent);

    /**
     * @brief Set the end position of the queue
     *
     * @param newQueueEnd The new end position of the queue
     */
    void setQueueEnd(int newQueueEnd);

    /**
     * @brief Get the current input byte from the buffer
     *
     * @return The current input byte
     */
    byte getCurrentInput();

    /**
     * @brief Get a pointer to the buffer at a specific index
     *
     * @param index The index to get the buffer pointer for
     * @return A pointer to the buffer at the specified index
     */
    uint8_t *getBuffer(int index);
};

extern LedController controller;
