#ifndef Debounce_h
#define Debounce_h

#include "Arduino.h"

class Debounce
{
public:
  Debounce(uint8_t pin, uint8_t polarity, uint8_t delayTime = 20);
  ~Debounce();
  bool read();

private:
  uint8_t _pin;         // The pin for this input
  uint8_t _dbDelayTime; // Debounce delay time

  unsigned long _dbDelay; // The last time the pin was toggled

  uint8_t _state;     // The current state of the pin
  uint8_t _lastState; // the last state of the pin
  uint8_t _polarity;
};

#endif