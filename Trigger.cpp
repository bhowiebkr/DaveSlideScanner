#include "Trigger.h"

Trigger::Trigger()
{
    triggered_ = false;
    stop_ = true;
}

bool Trigger::was_triggered()
{
    if (triggered_)
    {
        reset();
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

void Trigger::reset()
{
    triggered_ = false;
    stop_ = true;
    timeout_ = 0; // or any value that indicates the timeout is not set
}

void Trigger::trigger(uint16_t delay)
{
    reset();
    timeout_ = millis() + delay;
    triggered_ = false;
    stop_ = false;
}