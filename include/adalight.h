/* adalight.h
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

#include "config.h"
#include "framestate.h"
#include "led_controller.h"
#include "statistics.h"

#if defined(LED_POWER_PIN)
#include "powercontrol.h"
#endif

class Adalight
{
  public:
#ifdef HS_MULTICORE
    static TaskHandle_t processDataHandle;
    static TaskHandle_t processSerialHandle;
    static xSemaphoreHandle i2sXSemaphore;
#endif

    /**
     * @brief separete thread for handling incoming data using cyclic buffer
     *
     * @param parameters
     */
    static void processDataTask(void *parameters);

    /**
     * @brief separate thread for handling serial communication and filling the
     * buffer
     *
     * @param parameters
     */
    static void processSerialTask(void *parameters);

    /**
     * @brief process incoming serial data
     *
     * @return true
     * @return false
     */
    static bool processSerial();

    /**
     * @brief process data from the buffer and prepare it for rendering
     *
     */
    static void processData();

    /**
     * @brief setup multi-core processing
     *
     */
#ifdef HS_MULTICORE
    static void setupMultiCore();
#endif

    /**
     * @brief initialize Adalight
     *
     */
    static void init();
};
