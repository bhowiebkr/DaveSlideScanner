#ifndef _TRIGGER_h
#define _TRIGGER_h

#include "Arduino.h"

class Trigger
{
public:
  Trigger();
  void trigger(uint16_t delay);
  void update();
  bool wasTriggered();

private:
  unsigned long timeout_;
  bool triggered_;
  bool stop_;
};

#endif