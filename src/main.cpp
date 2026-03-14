/* adalight.cpp
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

#include "calibration.h"
#include "config.h"
#include <Arduino.h>

#define SerialPort Serial

#ifdef LED_POWER_PIN
#include "powercontrol.h"
#endif

#include "adalight.h"

void setup()
{
    // Init serial port
    Serial.setRxBufferSize(MAX_BUFFER - 1);
    Serial.setTimeout(50);
    Serial.begin(SERIALCOM_SPEED);

    while (!Serial)
        continue;

    Adalight::init();

#if defined(LED_POWER_PIN)
    Serial.write("LED_POWER_PIN = ");
    Serial.println(LED_POWER_PIN);
    powerControl.init();
#endif

#ifdef HS_MULTICORE
    Adalight::setupMultiCore();
#endif
}

void loop()
{
#if !defined(HS_MULTICORE)
    Adalight::processSerial();
    Adalight::processData();
#endif
}
