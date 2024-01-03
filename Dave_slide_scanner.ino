#include <Arduino.h>
#include "Debounce.h"
#include <LiquidCrystal.h>
#include "Trigger.h"

// LCD initialization
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Define button values
int lcd_key = 0;
int adc_key_in = 0;

#define btnStart 0
#define btnStop 1
#define btnStep 2
#define btnNone 3

// Pins for projector, photo, and slide loading
static const uint8_t projector_pin = 11;
static const uint8_t photo_pin = 12;
static const int slide_load_pin = 13;

// Timings for various actions
int trigger_projector_ms = 1;  // Time for the relay to be on to trigger the projector
int slide_settle_ms = 900;     // Time for the slide to be settled after it's loaded
int trigger_photo_ms = 500;    // Time to trigger the camera to take the photo
int wait_photo_ms = 1000;      // Time after the camera took the photo before loading the next slide



// Trigger instance
Trigger trigger;

// Debounce for slide loaded input
static Debounce slideLoadedInput(slide_load_pin, 0, 2);

// Enum to represent different states of the scanner
enum ScannerStates {
  STOPPED,
  LOADING_SLIDE,
  WAITING_FOR_SLIDE,
  SLIDE_LOADED,
  TAKING_PHOTO,
  PHOTO_TAKEN,
  STEP
};

// Initialize scanner state to STOPPED
ScannerStates scannerState = STOPPED;

// Function to print to the LCD screen
void printToLCD(const char* text, int line) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16.16s", text);
  lcd.setCursor(0, line);
  lcd.print(buf);
}

// Function to handle the scanner state transitions and actions
void ScannerMachine() {
  trigger.update();
  int btn = get_button();

  switch (scannerState) {
    case STEP:
      // Transition to STOPPED state when trigger was activated
      if (trigger.was_triggered()) {
        scannerState = STOPPED;
        digitalWrite(projector_pin, LOW);
      }
      break;

    case STOPPED:
      // Display STOPPED status and transition to LOADING_SLIDE state when Start button is pressed
      printToLCD("Status: STOPPED", 0);
      if (btn == btnStart) {
        trigger.trigger(trigger_projector_ms);
        digitalWrite(projector_pin, HIGH);
        scannerState = LOADING_SLIDE;
      }
      break;

    case LOADING_SLIDE:
      // Display LOADING status and transition to WAITING_FOR_SLIDE state when slide loading trigger activated
      printToLCD("Status: LOADING", 0);
      if (trigger.was_triggered()) {
        digitalWrite(projector_pin, LOW);
        scannerState = WAITING_FOR_SLIDE;
      }
      break;

    case WAITING_FOR_SLIDE:
      // Display WAITING status and transition to SLIDE_LOADED state when slide is detected as loaded
      printToLCD("Status: WAITING", 0);
      if (slideLoadedInput.read()) {
        scannerState = SLIDE_LOADED;
        trigger.trigger(slide_settle_ms);
      }
      break;

    case SLIDE_LOADED:
      // Display LOADED status and transition to TAKING_PHOTO state when trigger activated
      printToLCD("Status: LOADED", 0);
      if (trigger.was_triggered()) {
        scannerState = TAKING_PHOTO;
        trigger.trigger(trigger_photo_ms);
        digitalWrite(photo_pin, HIGH);
      }
      break;

    case TAKING_PHOTO:
      // Display SHOOT status and transition to PHOTO_TAKEN state when trigger activated
      printToLCD("Status: SHOOT", 0);
      if (trigger.was_triggered()) {
        digitalWrite(photo_pin, LOW);
        scannerState = PHOTO_TAKEN;
        trigger.trigger(wait_photo_ms);
      }
      break;

    case PHOTO_TAKEN:
      // Display TAKEN status and transition to LOADING_SLIDE state when trigger activated
      printToLCD("Status: TAKEN", 0);
      if (trigger.was_triggered()) {
        trigger.trigger(trigger_projector_ms);
        digitalWrite(projector_pin, HIGH);
        scannerState = LOADING_SLIDE;
      }
      break;
  }
}

// Function to read the button input
int get_button() {
  adc_key_in = analogRead(0);

  // Analog thresholds to determine the pressed button
  if (adc_key_in > 1000)
    return btnNone;
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

  return btnNone;
}

// Arduino setup function
void setup() {
  // Initialize pin modes and set them to low
  pinMode(projector_pin, OUTPUT);
  pinMode(photo_pin, OUTPUT);
  pinMode(slide_load_pin, INPUT);
  digitalWrite(projector_pin, LOW);
  digitalWrite(photo_pin, LOW);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

// Arduino main loop
void loop() {
  lcd.setCursor(0, 0);
  ScannerMachine();
}
