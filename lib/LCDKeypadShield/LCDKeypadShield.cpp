#include "Arduino.h"
#include "LCDKeypadShield.h"
#include <LiquidCrystal.h>

LCDKeypadShield::LCDKeypadShield()
{
    adc_key_in_ = 0;
}

Btn LCDKeypadShield::read_buttons()
{
    adc_key_in_ = analogRead(0); // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    if (adc_key_in_ > 1000)
        return Btn::NONE; // We make this the 1st option for speed reasons since it will be the most likely result

    if (adc_key_in_ < 50)
        return Btn::RIGHT;
    if (adc_key_in_ < 250)
        return Btn::UP;
    if (adc_key_in_ < 450)
        return Btn::DOWN;
    if (adc_key_in_ < 650)
        return Btn::LEFT;
    if (adc_key_in_ < 850)
        return Btn::SELECT;

    return Btn::NONE; // when all others fail, return this...
}

void LCDKeypadShield::update()
{
    lcd->setCursor(0, 0);
    lcd->print(line0);
    lcd->setCursor(0, 1);
    lcd->print(line1);
}

void LCDKeypadShield::setup()
{
    lcd->begin(16, 2);
    lcd->setCursor(0, 0);
}