#include <Arduino.h>
#include <Debounce.h>
#include <LCDKeypadShield.h>
#include <Relay.h>

LCDKeypadShield lcdShield;
static const uint8_t projector_pin = 11;
static const uint8_t photo_pin = 12;
static const int slide_load_pin = 2;

static Relay prjRelay(projector_pin, HIGH);
static Relay photoRelay(photo_pin, LOW);
static Btn btn;

static Debounce slideLoadedInput(slide_load_pin, 0, 20);

int lastBtnState = 1;

void setup()
{

    Serial.begin(9600);
    lcdShield.setup();

    pinMode(slide_load_pin, INPUT_PULLUP);
}

void loop()
{
    // new state
    int slideLoaded = slideLoadedInput.read();
    Serial.println(slideLoaded);

    // update the display
    lcdShield.updateDisplay();
    prjRelay.update();
    photoRelay.update();

    // Read the current button pressed
    btn = lcdShield.read_buttons();

    switch (btn)
    {
    case Btn::RIGHT:
    {
        sprintf(lcdShield.line0, "%-16s", "RIGHT");
        prjRelay.oneshot(200, LOW);
        break;
    }
    case Btn::LEFT:
    {
        sprintf(lcdShield.line0, "%-16s", "LEFT");
        break;
    }
    case Btn::UP:
    {
        sprintf(lcdShield.line0, "%-16s", "UP");
        photoRelay.oneshot(50, HIGH);
        break;
    }
    case Btn::DOWN:
    {
        sprintf(lcdShield.line0, "%-16s", "DOWN");
        break;
    }
    case Btn::SELECT:
    {
        sprintf(lcdShield.line0, "%-16s", "SELECT");
        break;
    }
    case Btn::NONE:
    {
        sprintf(lcdShield.line0, "%-16s", "NONE");
        break;
    }
    }
}