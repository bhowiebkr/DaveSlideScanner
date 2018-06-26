#include "Arduino.h"
#include "Relay.h"

Relay::Relay(uint8_t pin, bool value)
{
    _pin = pin;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, value);
    _state = State::EXPIRED;
}

void Relay::oneshot(uint16_t milliseconds, bool value)
{
    _expireMillis = milliseconds + millis();
    _state = State::ACTIVE;
    digitalWrite(_pin, value);
}

void Relay::update()
{
    if (_state == State::ACTIVE)
    {
        if(millis() >= _expireMillis){
            _state = State::EXPIRED;
            digitalWrite(_pin, !digitalRead(_pin));
        }
    }
}