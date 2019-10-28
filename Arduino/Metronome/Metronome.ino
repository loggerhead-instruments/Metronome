// Metronome Switch Timer
// Loggerhead Instruments
// c2019  
// David Mann

/*
 To Do:
 - real-time clock
 - display
 - buttons
 - figure out setup flow
 - GPS
 - microSD (store log file)
 */


#include <Wire.h>
#include <SPI.h>
#include <SdFat.h>
#include <RTCZero.h>
#include "LowPower.h"

#define ledOn LOW
#define ledOff HIGH
#define ledGreen 5
#define ledRed 2
#define relay1 8
#define relay2 9
#define relay3 4
#define relay4 3
#define upButton A3
#define downButton A2
#define enterButton A1
#define pin13 13
#define pin12 12
#define pin6 6
#define pin7 7


SdFat sd;
File dataFile;

void setup() {
  pinMode(ledGreen, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(enterButton, INPUT_PULLUP);
  pinMode(pin13, OUTPUT);
  pinMode(pin12, OUTPUT);
  pinMode(pin6, OUTPUT);
  pinMode(pin7, OUTPUT);
  
  digitalWrite(ledGreen, ledOn);
  digitalWrite(ledRed, ledOff);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  digitalWrite(pin13, LOW);
  digitalWrite(pin12, LOW);
  digitalWrite(pin13, LOW);
  digitalWrite(pin14, LOW);
  

}

void loop() {
  // put your main code here, to run repeatedly:

}
