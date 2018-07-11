#ifndef _LCDKEYPADSHIELD_h
#define _LCDKEYPADSHIELD_h

#include "Arduino.h"
#include <LiquidCrystal.h>

enum Btn
{
  LEFT,
  RIGHT,
  UP,
  DOWN,
  SELECT,
  NONE
};

class LCDKeypadShield
{
public:
  LCDKeypadShield();
  Btn read_buttons();
  void update();
  void setup();

  LiquidCrystal *lcd = new LiquidCrystal(9, 8, 4, 5, 6, 7);

  char line0[17];
  char line1[17];

private:
  uint16_t adc_key_in_;
};

#endif