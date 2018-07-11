#include <Arduino.h>
#include <Debounce.h>
#include <LCDKeypadShield.h>
#include <Trigger.h>

LCDKeypadShield lcdShield;               // LCD Keypad Shield
static const uint8_t projector_pin = 11; // Output pin used to load the next slide on the projector
static const uint8_t photo_pin = 12;     // Output pin to take a photo
static const int slide_load_pin = 2;     // Input pin to know when a slide is loaded
static const int start_pin = 3;     // Input pin to know when a slide is loaded


// Timings
int trigger_projector_ms = 200; // Time for the relay to be on to trigger the projector
int slide_settle_ms = 300;      // Time for the slide to be settled after it's loaded
int trigger_photo_ms = 150;     // Time to trigger the camera to take the photo
int wait_photo_ms = 300;        // Time after the camera took the photo before loading the next slide
int total_images = 0;           // Total images taken before being reset

Trigger trigger;


static Debounce slideLoadedInput(slide_load_pin, 1, 20); // A Debounce to remove noise in on the input
static Debounce startBtn(start_pin, 1, 20); // A Debounce to remove noise in on the input


enum ScannerStates
{
  STOPPED,       // Waiting to start
  LOADING_SLIDE, //
  WAITING_FOR_SLIDE,
  SLIDE_LOADED,
  TAKING_PHOTO,
  PHOTO_TAKEN,

};

ScannerStates scannerState = STOPPED; // We start the state machine in the STOPPED state


void ScannerMachine()
{
  // Update the machine's parts
  trigger.update();
  lcdShield.update();

  // Trigger a reset
  if (lcdShield.read_buttons() == Btn::SELECT)
  {
    scannerState = STOPPED;
  }

  switch (scannerState)
  {
  case STOPPED:
  {
    if (lcdShield.read_buttons() == Btn::SELECT)
    {
      trigger.trigger(trigger_projector_ms);
      digitalWrite(projector_pin, LOW);
      scannerState = LOADING_SLIDE;
      Serial.println("State: LOADING_SLIDE");
    }
    break;
  }
  case LOADING_SLIDE:
  {
    if (trigger.wasTriggered())
    {
      digitalWrite(projector_pin, HIGH);
      scannerState = WAITING_FOR_SLIDE;
      Serial.println("State: WAITING_FOR_SLIDE");
    }

    break;
  }
  case WAITING_FOR_SLIDE:
  {
    if (slideLoadedInput.read())
    {
      scannerState = SLIDE_LOADED;
      trigger.trigger(slide_settle_ms);
      Serial.println("State: SLIDE_LOADED");
    }
    break;
  }
  case SLIDE_LOADED:
  {
    if (trigger.wasTriggered())
    {
      scannerState = TAKING_PHOTO;
      trigger.trigger(trigger_photo_ms);
      digitalWrite(photo_pin, HIGH);
      Serial.println("State: TAKING_PHOTO");
    }
    break;
  }
  case TAKING_PHOTO:
  {
    if (trigger.wasTriggered())
    {
      digitalWrite(photo_pin, LOW);
      scannerState = PHOTO_TAKEN;
      trigger.trigger(wait_photo_ms);
      Serial.println("State: PHOTO_TAKEN");
    }
    break;
  }
  case PHOTO_TAKEN:
  {
    if (trigger.wasTriggered())
    {
      total_images += 1;
      Serial.print("Images: ");
      Serial.println(total_images);

      trigger.trigger(trigger_projector_ms);
      digitalWrite(projector_pin, LOW);
      scannerState = LOADING_SLIDE;
      Serial.println("State: LOADING_SLIDE");
    }
    break;
  }
  }
}

void setup()
{

  Serial.begin(9600);
  lcdShield.setup();

  pinMode(projector_pin, OUTPUT);
  pinMode(photo_pin, OUTPUT);

  digitalWrite(projector_pin, HIGH);
  digitalWrite(photo_pin, LOW);

  pinMode(slide_load_pin, INPUT_PULLUP);
}

void loop()
{
  ScannerMachine();
}