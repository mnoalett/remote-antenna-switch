#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Events.h"

static long shouldReboot;
static long rebootRequestTime;
static long timeoutLedBlink;
static long startTimeoutLedBlink;
static bool enabledTimerLedBlink;

Events::Events() {}

void Events::startTimerLedBlink(long timeout)
{
    timeoutLedBlink = timeout;
    startTimeoutLedBlink = millis();
    enabledTimerLedBlink = true;
}

void Events::stopTimerLedBlink()
{
    enabledTimerLedBlink = false;
}

void Events::reboot()
{
    shouldReboot = true;
    rebootRequestTime = millis();
}

void Events::handleEvent()
{
    if (enabledTimerLedBlink && ((millis() - startTimeoutLedBlink) > timeoutLedBlink))
    {
        digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) ? LOW : HIGH);
        startTimerLedBlink(timeoutLedBlink);
    }

    if (shouldReboot && ((millis() - rebootRequestTime) > rebootDelay))
    {
        ESP.restart();
    }
}