// Metronome Switch Timer
// Loggerhead Instruments
// c2020
// David Mann

/*
 To Do:
 Requirements:
 turn the system ON every six hours for 40 mins. 
 System should be turned ON at 1:00, 7:00, 13:00, and 19:00
 GPS clock Sync at least once a day should be performed.

For each channel
- ON Time (HH:MM) (any time, and it should figure out which ones are valid)
- On Duration (minutes)
- ON Interval (hours)

OR

- List of Times and durations
 
 - real-time clock
 - buttons
 - GPS
 - microSD (store log file)
 */

#define metronomeVersion "2020-01-13"

#include <Wire.h>
#include <RTCZero.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED.h>

#define OLED_RESET -1
#define displayLine1 0
#define displayLine2 9
#define displayLine3 18
#define displayLine4 27
Adafruit_FeatherOLED display = Adafruit_FeatherOLED();
#define BOTTOM 25
#define BOTTOM 55

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
#define vSense A4 


volatile float voltage;

int rec_dur = 60;
int rec_int = 60;

/* Create an rtc object */
RTCZero rtc;
/* Change these values to set the current initial time and date */
volatile byte second = 0;
volatile byte minute = 0;
volatile byte hour = 17;
volatile byte day = 1;
volatile byte month = 1;
volatile byte year = 20;

boolean introPeriod=1;  //flag for introductory period; used for keeping LED on for a little while

// GPS
#define gpsSerial Serial1
float latitude = 0.0;
float longitude = 0.0;
char latHem, lonHem;
int gpsYear = 20, gpsMonth = 1, gpsDay = 4, gpsHour = 22, gpsMinute = 5, gpsSecond = 0;
int goodGPS = 0;
long gpsTimeOutThreshold = 120000;

void setup() {
  SerialUSB.begin(115200); // Serial monitor
  delay(1000);
  SerialUSB.println("Metronome");
  rtc.begin();
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
  pinMode(vSense, INPUT);
  
  digitalWrite(ledGreen, ledOn);
  digitalWrite(ledRed, ledOff);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  digitalWrite(pin13, LOW);
  digitalWrite(pin12, LOW);
  digitalWrite(pin6, LOW);
  digitalWrite(pin7, LOW);

  Wire.begin();

  displayOn();
  delay(140);
  cDisplay();
  display.println("Metronome");
  display.display();

  while(!goodGPS){
    gpsGetTimeLatLon();
    if(!goodGPS){
        Serial.println("Unable to get GPS");
        cDisplay();
        display.println();
        display.println("Wait for GPS");
        display.println("Do not deploy");
        display.display();
        delay(2000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

float readVoltage(){
  float vDivider = 0.5;
  float vReg = 3.3;
  float voltage = (float) analogRead(vSense) * vReg / (vDivider * 1024.0);
  return voltage;
}
