#ifndef Relay_h
#define Relay_h

#include "Arduino.h"

enum State
{
    EXPIRED,
    ACTIVE
};

class Relay
{
  public:
    Relay(uint8_t pin, bool value);
    void oneshot(uint16_t milliseconds, bool value);
    void update();

  private:
    uint8_t _pin;
    State _state;
    unsigned long _expireMillis;
};

#endif