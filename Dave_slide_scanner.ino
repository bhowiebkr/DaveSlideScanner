#include <Arduino.h>
#include "Debounce.h"
#include <LiquidCrystal.h>
#include "Trigger.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key = 0;
int adc_key_in = 0;

#define btnStart 0
#define btnStop 1
#define btnStep 2
#define btnNone 3

static const uint8_t projector_pin = 11;  // Output pin used to load the next slide on the projector
static const uint8_t photo_pin = 12;      // Output pin to take a photo
static const int slide_load_pin = 13;     // Input pin to know when a slide is loaded


// Timings
int trigger_projector_ms = 1;  // Time for the relay to be on to trigger the projector
int slide_settle_ms = 900;    // Time for the slide to be settled after it's loaded
int trigger_photo_ms = 500;     // Time to trigger the camera to take the photo
int wait_photo_ms = 1000;      // Time after the camera took the photo before loading the next slide
int total_images = 0;          // Total images taken before being reset

Trigger trigger;

int photo_num = 1;


static Debounce slideLoadedInput(slide_load_pin, 1, 20);  // A Debounce to remove noise in on the input


enum ScannerStates {
  STOPPED,        // Waiting to start
  LOADING_SLIDE,  //
  WAITING_FOR_SLIDE,
  SLIDE_LOADED,
  TAKING_PHOTO,
  PHOTO_TAKEN,
  STEP

};

ScannerStates scannerState = STOPPED;  // We start the state machine in the STOPPED state


void ScannerMachine() {
  // Update the machine's parts
  trigger.update();

  int btn = get_button();

  if (btn == btnStop) {
    scannerState = STOPPED;
  } else if (btn == btnStep) {
    lcd.print("Status: STEP");
    delay(400);
    trigger.trigger(trigger_projector_ms);
    digitalWrite(projector_pin, HIGH);
    scannerState = STEP;
  }


  switch (scannerState) {
    // Step State to the next slide but don't take a photo (manual mode)
    case STEP:
      {
        if (trigger.wasTriggered()) {
          scannerState = STOPPED;
          digitalWrite(projector_pin, LOW);
        }
      }
    
    // Stop state. Waiting for the button to start to run
    // When the button starts, we trigger the projector to load a slide
    case STOPPED:
      {
        lcd.print("Status: STOPPED");

        if (btn == btnStart) {
          delay(400);
          trigger.trigger(trigger_projector_ms);
          digitalWrite(projector_pin, HIGH);
          scannerState = LOADING_SLIDE;
        }
        break;
      }

    // Loading a slide to get ready for a photo state 
    // When the 
    case LOADING_SLIDE:
      {
        lcd.print("Status: LOADING");

        if (trigger.wasTriggered()) {
          digitalWrite(projector_pin, LOW);
          scannerState = WAITING_FOR_SLIDE;
        }

        break;
      }
    // waiting for slide state, 
    // when the slide has been loaded, take the photo
    case WAITING_FOR_SLIDE:
      {
        lcd.print("Status: WAITING");

        if (btn == btnStop) {
          lcd.print("Status: STOPPING");
          scannerState = STOPPED;
          photo_num = 1;
          lcd.print(sprintf("Slide: %-5d", photo_num));
        }


        if (slideLoadedInput.read()) {
          scannerState = SLIDE_LOADED;
          trigger.trigger(slide_settle_ms);
        }
        break;
      }
    case SLIDE_LOADED:
      {
        lcd.print("Status: LOADED");

        if (trigger.wasTriggered()) {
          scannerState = TAKING_PHOTO;
          trigger.trigger(trigger_photo_ms);
          digitalWrite(photo_pin, HIGH);
        }
        break;
      }
    case TAKING_PHOTO:
      {
        lcd.print("Status: SHOOT");

        if (trigger.wasTriggered()) {
          digitalWrite(photo_pin, LOW);
          scannerState = PHOTO_TAKEN;
          trigger.trigger(wait_photo_ms);
        }
        break;
      }
    case PHOTO_TAKEN:
      {
        lcd.print("Status: TAKEN");

        if (trigger.wasTriggered()) {
          total_images += 1;

          trigger.trigger(trigger_projector_ms);
          digitalWrite(projector_pin, HIGH);
          scannerState = LOADING_SLIDE;
          photo_num = photo_num + 1;
          lcd.setCursor(1, 0);
          lcd.print(sprintf("Slide: %-5d", photo_num));
          lcd.setCursor(0, 0);
        }
        break;
      }
  }
}

int get_button() {
  adc_key_in = analogRead(0);

  if (adc_key_in > 1000)
    return btnNone;  // We make this the 1st option for speed reasons since it will be the most likely result
  if (adc_key_in < 50)
    return btnNone;
  if (adc_key_in < 195)
    return btnStart;
  if (adc_key_in < 380)
    return btnStop;
  if (adc_key_in < 555)
    return btnNone;
  if (adc_key_in < 790)
    return btnStep;

  return btnNone;  // when all others fail, return this...
}

void setup() {

  pinMode(projector_pin, OUTPUT);
  pinMode(photo_pin, OUTPUT);

  pinMode(slide_load_pin, INPUT);

  digitalWrite(projector_pin, LOW);
  digitalWrite(photo_pin, LOW);

  lcd.begin(16, 2);  // start the LCD
  lcd.setCursor(0, 0);

  lcd.setCursor(1, 0);
  lcd.print(sprintf("Slide: %-5d", photo_num));
  lcd.setCursor(0, 0);
}

void loop() {
  lcd.setCursor(0, 0);
  ScannerMachine();
}