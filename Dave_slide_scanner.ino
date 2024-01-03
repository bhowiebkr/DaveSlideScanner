#include <Arduino.h>
#include "Debounce.h"
#include <LiquidCrystal.h>


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



// Debounce for slide loaded input
static Debounce slideLoadedInput(slide_load_pin, 0, 20);

// Enum to represent different states of the scanner
enum ScannerStates {
  STOPPED,
  LOADING_SLIDE,
  STEP,
};

// Initialize scanner state to STOPPED
ScannerStates scannerState = STOPPED;

// Function to print to the LCD screen
void printToLCD(const char *text, int line) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16.16s", text);
  lcd.setCursor(0, line);
  lcd.print(buf);
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
  int btn = get_button();
  bool shouldBreak = false;  // Set the flag to exit the while loop and switch case

  switch (scannerState) {
    case STOPPED:
      // Display STOPPED status and transition to LOADING_SLIDE state when Start button is pressed
      printToLCD("Status: STOPPED", 0);
      if (btn == btnStart) {
        scannerState = LOADING_SLIDE;
      }
      if (btn == btnStep) {
        scannerState = STEP;
      }
      break;
    case STEP:
      printToLCD("Status: STEPPING", 0);
      digitalWrite(projector_pin, HIGH);
      delay(5);
      digitalWrite(projector_pin, LOW);
      scannerState = STOPPED;
      delay(200);
      break;


    case LOADING_SLIDE:
      printToLCD("Status: LOADING", 0);
      digitalWrite(projector_pin, HIGH);
      delay(5);
      digitalWrite(projector_pin, LOW);

      delay(100);  // time so the previous slide get unloaded

      printToLCD("Status: WAITING", 0);
      int count = 1;
      while (!slideLoadedInput.read()) {
        delay(10);
        char btnText[17];
        snprintf(btnText, sizeof(btnText), "Time passed: %d", count);
        printToLCD(btnText, 1);
        count += 1;
        if (count > 80) {
          scannerState = STOPPED;
          shouldBreak = true;  // Set the flag to exit the while loop and switch case
          snprintf(btnText, sizeof(btnText), "");
          printToLCD(btnText, 1);
          break;
        }
      }

      if (!shouldBreak) {
        delay(200);  // delay so the slide settles

        // Take the photo (delay is how long to have the shutter open to focus)
        printToLCD("Status: SHOOTING", 0);
        digitalWrite(photo_pin, HIGH);
        delay(1000); 
        digitalWrite(photo_pin, LOW);

        // wait for the photo to be taken
        printToLCD("Status: CAMERA WAIT", 0);
        delay(2000);
      }
  }
}
