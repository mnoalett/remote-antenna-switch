#ifndef EVENTS_H
#define EVENTS_H

#include <Arduino.h>

class Events
{
public:
    static Events &getInstance()
    {
        static Events instance;
        return instance;
    }

    void handleEvent();
    void reboot();
    void startTimerLedBlink(long timeout);
    void stopTimerLedBlink();

private:
    const int rebootDelay = 5000;
    Events();
};

#endif