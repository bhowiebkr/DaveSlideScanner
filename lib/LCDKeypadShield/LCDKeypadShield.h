#ifndef LCDKeypadShield_h
#define LCDKeypadShield_h

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
  void updateDisplay();
  void setup();

  LiquidCrystal *lcd = new LiquidCrystal(8, 9, 4, 5, 6, 7);

  char line0[17];
  char line1[17];

private:
  uint16_t  _adc_key_in;
};

#endif