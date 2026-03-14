#include "powercontrol.h"

#if defined(LED_POWER_PIN)

PowerControl powerControl;

void PowerControl::init()
{
    pinMode(LED_POWER_PIN, OUTPUT);
    lastPowerOffResetTimestamp = millis();
    powerOff();
}

void PowerControl::powerOn()
{
    if (currentPowerPinMode != HIGH)
    {
        currentPowerPinMode = HIGH;
        digitalWrite(LED_POWER_PIN, currentPowerPinMode);
    }
}

void PowerControl::powerOff()
{
    if (currentPowerPinMode != LOW)
    {
        currentPowerPinMode = LOW;
        digitalWrite(LED_POWER_PIN, currentPowerPinMode);
    }
}

void PowerControl::update(bool hasData)
{
    if (hasData)
    {
        powerOn();
        lastPowerOffResetTimestamp = millis();
    }
    else if (millis() - lastPowerOffResetTimestamp > POWER_OFF_PERIOD)
    {
        powerOff();
    }
}

#endif
