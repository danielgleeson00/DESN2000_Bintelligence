/*
Reads Analog input from WISblock and performs logic operations to determine which LED to light up
Created by Daniel Gleeson
10/11/22

Based of publicly available code from https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/
/*
  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - potentiometer connected to analog pin 0.
    Center pin of the potentiometer goes to the analog pin.
    side pins of the potentiometer go to +5V and ground
  - LED connected from digital pin 9 to ground through 220 ohm resistor

  created 29 Dec. 2008
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInOutSerial
*/

// Bring in libraries
#include <Arduino.h>
#include <LoRaWan-RAK4630.h>
#include <Wire.h>
#include <Adafruit_GFX.h>   //Click here to get the library: http://librarymanager/All#Adafruit_GFX
#include <Adafruit_SSD1306.h>   //Click here to get the library: http://librarymanager/All#Adafruit_SSD1306

const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to

// IO ports that the LEDs are attached to
const int analogOutPinGreen = 10;
const int analogOutPinOrange = 9;

int sensorValue = 0;  // value read from the pot
int VBAT_MAX = 4.3;
int threshold;
bool isGreen, isOrange;


void setup() {

  // initialize serial communications at 9600 bps:
    Serial.begin(9600);
      // Initialize built in green LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Wait for USB Serial to be ready or terminal to be connected
    time_t timeout = millis(); // Timeout in case the system runs on its own
    // Waiting for Serial
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            // Blink the LED to show that we are alive
            delay(100);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        else
        {
            // Timeout while waiting for USB Serial
            break;
        }
    }
}

void loop() {

  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  threshold = 1023*4/3;

  // print the results to the Serial Monitor:
  Serial.print("sensor = ");
  Serial.print(sensorValue);

  isGreen = (sensorValue > threshold);
  // The 100 eliminates noise and battery check wont function if the wisblock doesn't have power.
  isOrange = (sensorValue <= threshold) && (sensorValue > 100); 

  if (isGreen){
      analogWrite(analogOutPinGreen, HIGH);
      analogWrite(analogOutPinOrange, LOW);
      Serial.print("Above 50%");
  }
  if (isOrange){
      analogWrite(analogOutPinOrange, HIGH);
      analogWrite(analogOutPinGreen, LOW);
      Serial.print("Below 50%");
  }
  
  // wait 1 second before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(1000);
}
