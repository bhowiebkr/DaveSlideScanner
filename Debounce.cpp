#include "Arduino.h"
#include "Debounce.h"

Debounce::Debounce(uint8_t pin, uint8_t polarity, uint8_t delayTime)
{
    _pin = pin;
    _dbDelay = delayTime;
    _polarity = polarity;
    pinMode(_pin, INPUT_PULLUP);
}
Debounce::~Debounce()
{
}

bool Debounce::read()
{
    // Get the current state and time
    uint8_t reading = digitalRead(_pin);

    // if the state changed due to noise
    if (reading != _lastState)
    {
        _lastState = reading;
        _dbDelayTime = millis();
    }

    if (millis() - _dbDelayTime > _dbDelay)
    {
        _state = _lastState;
        
        if (_state == _polarity)
        {
            return true;
        }
    }
    return false;
}