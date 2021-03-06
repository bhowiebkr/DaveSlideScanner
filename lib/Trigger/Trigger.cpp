#include "Trigger.h"
#include "Arduino.h"

Trigger::Trigger()
{
    triggered_ = false;
    stop_ = true;
}

bool Trigger::wasTriggered()
{
    if (triggered_)
    {
        triggered_ = false;
        stop_ = true;
        return true;
    }
    return false;
}

void Trigger::update()
{
    if (stop_)
    {
        return;
    }

    if ((long)(millis() - timeout_) >= 0)
    {
        triggered_ = true;
    }
}

void Trigger::trigger(uint16_t delay)
{
    timeout_ = millis() + delay;
    triggered_ = false;
    stop_ = false;
}