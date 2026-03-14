#include "led_controller.h"

LedController controller;

int LedController::getLedsNumber()
{
    return ledsNumber;
}

ILedDriver *LedController::getLedStrip1()
{
    return ledStrip1;
}

#if defined(SECOND_SEGMENT_START_INDEX)
ILedDriver *LedController::getLedStrip2()
{
    return ledStrip2;
}
#endif

void LedController::initLedStrip(int count)
{
    if (ledStrip1 != nullptr)
    {
        delete ledStrip1;
        ledStrip1 = nullptr;
    }

#if defined(SECOND_SEGMENT_START_INDEX)
    if (ledStrip2 != nullptr)
    {
        delete ledStrip2;
        ledStrip2 = nullptr;
    }
#endif

    ledsNumber = count;

#if defined(SECOND_SEGMENT_START_INDEX)
    if (ledsNumber > SECOND_SEGMENT_START_INDEX)
    {
        ledStrip1 = new LED_DRIVER(SECOND_SEGMENT_START_INDEX);
        ledStrip2 = new LED_DRIVER2(ledsNumber - SECOND_SEGMENT_START_INDEX);
    }
#else
    ledStrip1 = new LED_DRIVER(ledsNumber);
#endif
}

bool LedController::hasLateFrameToRender()
{
    return readyToRender;
}

void LedController::dropLateFrame()
{
    readyToRender = false;
}

bool LedController::canRender(bool newFrame)
{
    if (newFrame)
        readyToRender = true;

#if defined(SECOND_SEGMENT_START_INDEX)
    return readyToRender && (ledStrip1 != nullptr && ledStrip1->canShow()) &&
           !(ledStrip2 != nullptr && !ledStrip2->canShow());
#else
    return readyToRender && ledStrip1 != nullptr && ledStrip1->canShow();
#endif
}

void LedController::renderLeds()
{
#if defined(SECOND_SEGMENT_START_INDEX)
    readyToRender = false;

    ledStrip1->show(false);
    if (ledStrip2 != nullptr)
        ledStrip2->show(false);
#else
    readyToRender = false;
    ledStrip1->show(false);
#endif
}

bool LedController::setStripPixel(uint16_t pix, RgbwColor &inputColor)
{
    if (pix < ledsNumber)
    {
#if defined(SECOND_SEGMENT_START_INDEX)
        if (pix < SECOND_SEGMENT_START_INDEX)
            ledStrip1->setPixelColor(pix, inputColor);
        else
        {
#if defined(SECOND_SEGMENT_REVERSED)
            ledStrip2->setPixelColor(ledsNumber - pix - 1, inputColor);
#else
            ledStrip2->setPixelColor(pix - SECOND_SEGMENT_START_INDEX, inputColor);
#endif
        }
#else
        ledStrip1->setPixelColor(pix, inputColor);
#endif
    }

    return (pix + 1 < ledsNumber);
}

bool LedController::isAtEndOfQueue()
{
    return queueCurrent == queueEnd;
}

int LedController::getQueueCurrent()
{
    return queueCurrent;
}

int LedController::getQueueEnd()
{
    return queueEnd;
}

void LedController::setQueueCurrent(int newQueueCurrent)
{
    queueCurrent = newQueueCurrent;
}

void LedController::setQueueEnd(int newQueueEnd)
{
    queueEnd = newQueueEnd;
}

byte LedController::getCurrentInput()
{
    return buffer[queueCurrent++];
}

uint8_t *LedController::getBuffer(int index)
{
    return &buffer[index];
}
