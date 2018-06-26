#include "Arduino.h"
#include "LCDKeypadShield.h"
#include <LiquidCrystal.h>

LCDKeypadShield::LCDKeypadShield()
{
    _adc_key_in = 0;
}

Btn LCDKeypadShield::read_buttons()
{
    _adc_key_in = analogRead(0); // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (_adc_key_in > 1000)
        return Btn::NONE; // We make this the 1st option for speed reasons since it will be the most likely result

    if (_adc_key_in < 50)
        return Btn::RIGHT;
    if (_adc_key_in < 250)
        return Btn::UP;
    if (_adc_key_in < 450)
        return Btn::DOWN;
    if (_adc_key_in < 650)
        return Btn::LEFT;
    if (_adc_key_in < 850)
        return Btn::SELECT;

    return Btn::NONE; // when all others fail, return this...
}

void LCDKeypadShield::updateDisplay()
{
    lcd->setCursor(0, 0);
    lcd->print(line0);
    lcd->print(line1);
}

void LCDKeypadShield::setup()
{
    lcd->begin(16, 2);
    lcd->setCursor(0, 0);
}