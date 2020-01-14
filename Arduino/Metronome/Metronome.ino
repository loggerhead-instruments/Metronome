// Metronome Switch Timer
// Loggerhead Instruments
// c2020
// David Mann

/*
 To Do:
 - DS3232 RTC (optional)
 - measure current draw
 - check voltage divider is OK for 12 V input
 - sleep GPS
 */

#define metronomeVersion 20200113

#define MAXTIMES 24
volatile int nTimes = 4;
int scheduleHour[] = {1,7,13,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //start hour
int scheduleMinute[] = {0,0,0,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // start minute
float scheduleFracHour[MAXTIMES];
int duration[] = {40,40,40,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // duration on in minutes

#include <SPI.h>
#include <Wire.h>
#include <RTCZero.h>
#include <SdFat.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
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

// SD file system
SdFat sd;
File dataFile;
int sdFlag = 1; // =0 if can't see sd

void setup() {
  SerialUSB.begin(115200); // Serial monitor
  delay(5000);
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
  digitalWrite(relay1, LOW);
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

 if (!sd.begin(10, SPI_FULL_SPEED)) {
    SerialUSB.println("Card failed");
    display.println("No SD Card");
    sdFlag = 0;
    display.display();
    delay(5000);
  }

  while(!goodGPS){
    gpsGetTimeLatLon();
    if(!goodGPS){
        SerialUSB.println("Unable to get GPS");
        cDisplay();
        display.println("GPS No Fix");
        display.setTextSize(1);
        display.println("Do not deploy");
        display.display();
        delay(2000);
    }
  }
  rtc.setTime(gpsHour, gpsMinute, gpsSecond);
  rtc.setDate(gpsDay, gpsMonth, gpsYear);

  if(sdFlag){
    logFileHeader();
    int nLines = loadSchedule();
    if(nLines>0) nTimes = nLines;
  }

  manualSettings();
  
  for(int i=0; i<nTimes; i++){
    scheduleFracHour[i] = scheduleHour[i] + (scheduleMinute[i]/60.0);
  }
}

void loop() {
  // get next wake time from list based on current time
  getTime();
  int nextOnTimeIndex = getNextOnTime();
  printTime();
  SerialUSB.print("Next Start:");
  SerialUSB.print(scheduleHour[nextOnTimeIndex]);SerialUSB.print(":");
  SerialUSB.print(scheduleMinute[nextOnTimeIndex]);

  cDisplay();
  display.println("Sleeping");
  display.setTextSize(1);
  display.print("Next:");
  display.print(scheduleHour[nextOnTimeIndex]); display.print(":");
  printDigits(scheduleMinute[nextOnTimeIndex]);
  display.println();
  display.print(duration[nextOnTimeIndex]); display.println(" minutes");
  
  displayVoltage();
  
  display.display();

  
  // set alarm and sleep
  rtc.setAlarmTime(scheduleHour[nextOnTimeIndex],scheduleMinute[nextOnTimeIndex], 0);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
  rtc.attachInterrupt(alarmMatch);
  rtc.standbyMode();

  // ... Sleeping here ...

  // ... Awake ...
  rtc.detachInterrupt();
  rtc.disableAlarm();

  cDisplay();
  display.println("On");
  display.setTextSize(1);
  display.print(duration[nextOnTimeIndex]);
  display.println(" minutes");
  displayVoltage();
  display.display();
  // turn on all 4 channels
  relayOn();
  if(sdFlag) logEntry(1);

  // sleep 1 minute at a time and flash led
  rtc.setAlarmSeconds(0);
  for(int i = 0; i<duration[nextOnTimeIndex]; i++){
    getTime();
    int alarmMinute = minute + 1;
    if(minute>59) minute = 0;
    rtc.setAlarmMinutes(minute + 1);
    rtc.enableAlarm(rtc.MATCH_MMSS);
    rtc.attachInterrupt(alarmMatch);
    rtc.standbyMode();
    rtc.detachInterrupt();
    digitalWrite(ledGreen, ledOn);
    delay(100);
    digitalWrite(ledGreen, ledOff);
  }
  relayOff(); // turn off
  if(sdFlag) logEntry(0);
  cDisplay();
  display.println("GPS");
  display.setTextSize(1);
  display.print("Clock Update");
  display.display();
  updateGpsTime();  // update real-time clock with GPS time

}

float readVoltage(){
  float vDivider = 0.5;
  float vReg = 3.3;
  float voltage = (float) analogRead(vSense) * vReg / (vDivider * 1024.0);
  return voltage;
}

int updateGpsTime(){
  gpsGetTimeLatLon();
  if(goodGPS){
    rtc.setTime(gpsHour, gpsMinute, gpsSecond);
    rtc.setDate(gpsDay, gpsMonth, gpsYear);
    if(sdFlag) logEntry(2);
  }
  return(goodGPS);
}

void logEntry(int relayStatus){
   getTime();
   // log file
   float voltage = readVoltage();
   if(File logFile = sd.open("LOG.CSV",  O_CREAT | O_APPEND | O_WRITE)){
      char timestamp[30];
      sprintf(timestamp,"%d-%02d-%02dT%02d:%02d:%02d", year+2000, month, day, hour, minute, second);
      logFile.print(timestamp);
      logFile.print(',');
      logFile.print(voltage); 
      logFile.print(',');
      logFile.print(metronomeVersion);
      logFile.print(',');
      logFile.print(relayStatus);
      logFile.print(',');
      logFile.print(latitude);
      logFile.print(',');
      logFile.println(longitude);
      logFile.close();
   }
}

void logFileHeader(){
  if(File logFile = sd.open("LOG.CSV",  O_CREAT | O_APPEND | O_WRITE)){
      logFile.println("Datetime,Voltage,Version,Status,Latitude,Longitude");
      logFile.close();
   }
}

int getNextOnTime(){
  float curHour = hour + (minute/60.0);
  float difHour[MAXTIMES];
  // calc time to next hour from list
  SerialUSB.print("nTimes:"); SerialUSB.println(nTimes);
  SerialUSB.print("Cur hour:"); SerialUSB.println(curHour);
  SerialUSB.println("Frac Hour   Dif Hour");
  for(int i=0; i<nTimes; i++){
    difHour[i] = scheduleFracHour[i] - curHour;
    SerialUSB.print(scheduleFracHour[i]); SerialUSB.print("  ");
    SerialUSB.println(difHour[i]);
  } 

  // what is minimum positive value
  float minDif = 25;
  int nextIndex;
  for(int i=0; i<nTimes; i++){
    if(difHour[i]>0){
      if(difHour[i]<minDif) {
        minDif = difHour[i];
        nextIndex = i;
      }
    }
  }

  // no positive values; so pick most negative one
  if(minDif==25){
    for(int i=0; i<nTimes; i++){
      if(difHour[i]<minDif){
        minDif = difHour[i];
        nextIndex = i;
      }
    }
  }
  return nextIndex;
}

void relayOn(){
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
}

void relayOff(){
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
}

void alarmMatch()
{
  digitalWrite(LED_BUILTIN, HIGH);
}
