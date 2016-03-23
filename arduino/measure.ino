/**
 * Feedback Delay measurement program
 * 
 * How to use:
 * - Connect all components to the Arduino
 * - Connect the Arduino to the PC
 * - Connect the smartphone to the same ground as the measuring device
 * - Upload the sketch to the Arduino
 * - Open the Serial Monitor
 * - It will run a self test, triggering on and off
 * - As soon as the trigger starts working reliably, send anything over the Serial connection
 *   (i.e. type any character into the Serial Monitor input box and hit Send)
 * - The measurement routine will start
 * - Every measurement's result, the delay in nanoseconds, will be printed on one line.
 *
 * Author: Paul Grau <paul@graycoding.com>, 2016
 */
#include "pins_arduino.h"
#include <Mouse.h>
 
const short PIN_PHOTOCELL = A5;
const short PIN_LED = LED_BUILTIN;
const short PIN_TRIGGER = 2;
const short TRIGGER_MODE = 1; // 0 = Mouse click, 1 = pull PIN_TRIGGER to ground

long aval = 0; // Last analog value reading
unsigned long mean = 0; // Rolling mean
unsigned long meansq = 0; // Rolling mean square
unsigned int variance; // Current variance
const short variance_threshold = 6000; // Threshold to detect change
boolean detected_change = false;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  // Setup ADC interrupts
  DIDR0 = 0x3F; // disbale digital inputs on analog pins
  ADMUX = (INTERNAL << 6) | (analogPinToChannel(PIN_PHOTOCELL-18) & 0x07); // select reference and input
  ADCSRA = 0xA8 | 0b111; // ADC, interrupt enabled, prescaler = 128
  ADCSRB = 0x40; // AD channels MUX on, free running mode
  bitWrite(ADCSRA, 6, 1); // Start the conversion by setting bit 6 (ADSC)
  sei(); // enable interrupts
  
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection before resuming

  if (TRIGGER_MODE == 1) {
    // Trigger input a few times
    Serial.print("Self test: touch trigger on Pin ");
    Serial.println(PIN_TRIGGER);
    Serial.println("Send anything over Serial to device as soon as touches are triggered.");
    
    while(1) {
      trigger_input(true);
      digitalWrite(PIN_LED, HIGH); // LED on
      delay(400);
      trigger_input(false);   
      digitalWrite(PIN_LED, LOW); // LED off 
      delay(400);
      
      if (Serial.available() > 0) break;
    }
  }

  if (TRIGGER_MODE == 0) {
    Serial.println("Device acts as a HID now!");
    Serial.println("To stop, just disconnect USB.");
    Mouse.begin();
  }

  // Test photo diode
  Serial.print("Make sure photodiode is connected to PIN A");
  Serial.println(PIN_PHOTOCELL-18);
  delay(500);
}

/**
 * Interrupt routine for ADC ready 
 */
ISR(ADC_vect) {
  aval = ADCL;        // store lower byte ADC
  aval += ADCH << 8;  // store higher bytes ADC
  update_stats();
}

/**
 * Update statistical values
 */
void update_stats() {
  unsigned long aval_shifted = aval + 500;
  mean = mean*0.75 + (aval_shifted >> 2);
  meansq = meansq*0.75 + ((aval_shifted*aval_shifted) >> 2);
  variance = meansq - mean*mean;
  detected_change = detected_change || detect_change();
}

/**
 * Detect a change when variance exceeds threshold
 */
int detect_change() {
  return (variance > variance_threshold);
}

/**
 * Trigger an input event
 */
void trigger_input(boolean on) {
  // Touch trigger
  if (TRIGGER_MODE == 1) {
    if (on) {
       pinMode(PIN_TRIGGER, OUTPUT);
       digitalWrite(PIN_TRIGGER, LOW); // touch
    } else {
       pinMode(PIN_TRIGGER, INPUT); // touch up
    }
  }
  // USB Mouse trigger
  if (TRIGGER_MODE == 0) {
    if (on) {
      Mouse.press();
    } else {
      Mouse.release();
    }
  }
}

/**
 * Measurement routine
 */
void measure() {
  delay(400); // Wait a few samples to recalibrate mean

  // Start measurement
  detected_change = false;
  digitalWrite(PIN_LED, HIGH); // LED on
  trigger_input(true);
  unsigned long measure_start = micros();
  
  while (!detected_change) {
    // Keep sampling until threshold is reached
    delayMicroseconds(1);
  }

  // Stop measurement
  unsigned long measure_duration = micros() - measure_start;
  digitalWrite(PIN_LED, LOW); // LED off
  Serial.println(measure_duration);
  trigger_input(false);
}

void loop() {
  measure();
}
