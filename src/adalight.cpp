#include "adalight.h"

#include "config.h"
#include "framestate.h"
#include "led_controller.h"
#include "statistics.h"

#if defined(LED_POWER_PIN)
#include "powercontrol.h"
#endif

#if defined(HS_MULTICORE)
TaskHandle_t Adalight::processDataHandle = nullptr;
TaskHandle_t Adalight::processSerialHandle = nullptr;
xSemaphoreHandle Adalight::i2sXSemaphore = nullptr;
#endif

void Adalight::processDataTask(void *parameters)
{
    for (;;)
    {
        xSemaphoreTake(i2sXSemaphore, portMAX_DELAY);
        processData();
    }
}

void Adalight::processSerialTask(void *parameters)
{
    for (;;)
    {
        if (processSerial() || !controller.isAtEndOfQueue())
            xSemaphoreGive(i2sXSemaphore);
        yield();
    }
}

bool Adalight::processSerial()
{
    uint16_t incomingSize = min(Serial.available(), MAX_BUFFER - 1);

    if (incomingSize > 0)
    {
        if (controller.getQueueEnd() + incomingSize < MAX_BUFFER)
        {
            Serial.read(controller.getBuffer(controller.getQueueEnd()), incomingSize);
            controller.setQueueEnd(controller.getQueueEnd() + incomingSize);
        }
        else
        {
            int left = MAX_BUFFER - controller.getQueueEnd();
            Serial.read(controller.getBuffer(controller.getQueueEnd()), left);
            Serial.read(controller.getBuffer(0), incomingSize - left);
            controller.setQueueEnd(incomingSize - left);
        }
    }

#if defined(LED_POWER_PIN)
    powerControl.update(incomingSize > 0);
#endif

    return (incomingSize > 0);
}

void Adalight::processData()
{
    unsigned long currentTime = millis();
    unsigned long deltaTime = currentTime - statistics.getStartTime();

    statistics.handle(currentTime, deltaTime, !controller.isAtEndOfQueue());

    if (statistics.getStartTime() + 5000 < millis())
    {
        frameState.setState(AwaProtocol::HEADER_A);
    }

    if (controller.hasLateFrameToRender())
    {
        if (controller.canRender(false))
        {
            statistics.increaseShow();
            controller.renderLeds();
        }
    }

    while (!controller.isAtEndOfQueue())
    {
        byte input = controller.getCurrentInput();

        if (controller.getQueueCurrent() >= MAX_BUFFER)
        {
            controller.setQueueCurrent(0);
            yield();
        }

        switch (frameState.getState())
        {
        case AwaProtocol::HEADER_A:
            frameState.setProtocolVersion2(false);
            if (input == 'A')
                frameState.setState(AwaProtocol::HEADER_w);
            break;

        case AwaProtocol::HEADER_w:
            if (input == 'w')
                frameState.setState(AwaProtocol::HEADER_a);
            else
                frameState.setState(AwaProtocol::HEADER_A);
            break;

        case AwaProtocol::HEADER_a:
            if (input == 'a')
                frameState.setState(AwaProtocol::HEADER_HI);
            else if (input == 'A')
            {
                frameState.setState(AwaProtocol::HEADER_HI);
                frameState.setProtocolVersion2(true);
            }
            else
                frameState.setState(AwaProtocol::HEADER_A);
            break;

        case AwaProtocol::HEADER_HI:
            statistics.increaseTotal();
            frameState.init(input);
            controller.dropLateFrame();
            frameState.setState(AwaProtocol::HEADER_LO);
            break;

        case AwaProtocol::HEADER_LO:
            frameState.computeCRC(input);
            frameState.setState(AwaProtocol::HEADER_CRC);
            break;

        case AwaProtocol::HEADER_CRC:
            if (frameState.getCRC() == input)
            {
                uint16_t ledSize = frameState.getCount() + 1;
                if (ledSize > 4096)
                    frameState.setState(AwaProtocol::HEADER_A);
                else
                {
                    if (ledSize != controller.getLedsNumber())
                        controller.initLedStrip(ledSize);
                    frameState.setState(AwaProtocol::RED);
                }
            }
            else if (frameState.getCount() == 0x2aa2 && (input == 0x15 || input == 0x35))
            {
                statistics.print(currentTime, processDataHandle, processSerialHandle);
#if defined(NEOPIXEL_RGBW)
                calibration.print();
#endif

                if (input == 0x15)
                    Serial.println(HELLO_MESSAGE);
                delay(10);

                currentTime = millis();
                statistics.reset(currentTime);
                frameState.setState(AwaProtocol::HEADER_A);
            }
            else
                frameState.setState(AwaProtocol::HEADER_A);
            break;

        case AwaProtocol::RED:
            frameState.color.R = input;
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::GREEN);
            break;

        case AwaProtocol::GREEN:
            frameState.color.G = input;
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::BLUE);
            break;

        case AwaProtocol::BLUE:
            frameState.color.B = input;
            frameState.addFletcher(input);

#ifdef NEOPIXEL_RGBW
            frameState.rgb2rgbw();
#endif

            if (controller.setStripPixel(frameState.getCurrentLedIndex(), frameState.color))
            {
                frameState.setState(AwaProtocol::RED);
            }
            else
            {
                if (frameState.isProtocolVersion2())
                    frameState.setState(AwaProtocol::VERSION2_GAIN);
                else
                    frameState.setState(AwaProtocol::FLETCHER1);
            }

            break;

        case AwaProtocol::VERSION2_GAIN:
            calibration.setGain(input);
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::VERSION2_RED);
            break;

        case AwaProtocol::VERSION2_RED:
            calibration.setRed(input);
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::VERSION2_GREEN);
            break;

        case AwaProtocol::VERSION2_GREEN:
            calibration.setGreen(input);
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::VERSION2_BLUE);
            break;

        case AwaProtocol::VERSION2_BLUE:
            calibration.setBlue(input);
            frameState.addFletcher(input);
            frameState.setState(AwaProtocol::FLETCHER1);
            break;

        case AwaProtocol::FLETCHER1:
            if (input != frameState.getFletcher1())
                frameState.setState(AwaProtocol::HEADER_A);
            else
                frameState.setState(AwaProtocol::FLETCHER2);
            break;

        case AwaProtocol::FLETCHER2:
            if (input != frameState.getFletcher2())
                frameState.setState(AwaProtocol::HEADER_A);
            else
                frameState.setState(AwaProtocol::FLETCHER_EXT);
            break;

        case AwaProtocol::FLETCHER_EXT:
            if (input == frameState.getFletcherExt())
            {
                statistics.increaseGood();

                if (controller.canRender(true))
                {
                    statistics.increaseShow();
                    controller.renderLeds();
                }

#if defined(NEOPIXEL_RGBW)
                calibration.prepare();
#endif

                currentTime = millis();
                deltaTime = currentTime - statistics.getStartTime();
                statistics.handle(currentTime, deltaTime, true);
                yield();
            }

            frameState.setState(AwaProtocol::HEADER_A);
            break;
        }
    }
}

void Adalight::setupMultiCore()
{
    i2sXSemaphore = xSemaphoreCreateBinary();

    xTaskCreatePinnedToCore(processDataTask, "ProcessDataTask", 4096, NULL, 1, &processDataHandle, 0);

    xTaskCreatePinnedToCore(processSerialTask, "ProcessSerialTask", 4096, NULL, 1, &processSerialHandle, 1);
}

void Adalight::init()
{
#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
#ifdef NEOPIXEL_RGBW
#ifdef COLD_WHITE
    calibration.setParamsAndPrepare(0xFF, 0xA0, 0xA0, 0xA0);
#else
    calibration.setParamsAndPrepare(0xFF, 0xB0, 0xB0, 0x70);
#endif
#endif
#endif

#if !defined(CONFIG_IDF_TARGET_ESP32S2)
    Serial.println(HELLO_MESSAGE);

#if defined(SECOND_SEGMENT_START_INDEX)
    Serial.write("SECOND_SEGMENT_START_INDEX = ");
    Serial.println(SECOND_SEGMENT_START_INDEX);
#endif

// Colorspace/Led type info
#if defined(NEOPIXEL_RGBW) || defined(NEOPIXEL_RGB)
#ifdef NEOPIXEL_RGBW
#ifdef COLD_WHITE
    Serial.println("FastLED SK6812 cold GRBW. ");
#else
    Serial.println("FastLED SK6812 neutral GRBW. ");
#endif
    calibration.print();
#else
    Serial.println("FastLED ws281x type (GRB).");
#endif
#elif defined(SPILED_APA102)
    Serial.println("SPI APA102 compatible type (BGR).");
#elif defined(SPILED_WS2801)
    Serial.println("SPI WS2801 (RBG).");
#endif

    delay(50);
#endif
}
