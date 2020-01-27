#define setStart 0
#define setSchedule 1

boolean settingsChanged = 0;
int curMenuItem = 0;
volatile int maxMenuItem = 2;
unsigned long autoStartTime;
char *menuItem[] = {"Start",
                     "Schedule",
                     };

char *helpText[] = {"UP/DOWN:Scroll menu\nENTER:Start",
                    "ENTER:View Schedule\nUP/DN:scroll menu",
                    };

/* 
 *  DISPLAY FUNCTIONS
 */

void displayOn(){
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
}

void displayOff(){
  display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  display.print(":");
  printZero(digits);
  display.print(digits);
}

void printZero(int val){
  if(val<10) display.print('0');
}

void manualSettings(){
  boolean startRec = 0, startUp, startDown;
  autoStartTime = getUnixTime();
  // Main Menu Loop
   while(startRec==0){


    if(getUnixTime() - autoStartTime > 60){
      cDisplay();
      display.println("Starting..");
      display.setTextSize(1);
      display.display();
      delay(2000);
      startRec = 1;  //start recording 
    }
    
    
    // Check for button press
    boolean selectVal = digitalRead(upButton);

    if(selectVal==0){
      while(digitalRead(upButton)==0){
        delay(10); // wait until let go
      }
      curMenuItem++;
      if(curMenuItem>=maxMenuItem) curMenuItem = 0;
    }
    
    selectVal = digitalRead(downButton);
    if(selectVal==0){
      while(digitalRead(downButton)==0){
        delay(10); // wait until let go
      }
      curMenuItem--;
      if(curMenuItem<0) curMenuItem = maxMenuItem - 1;
    }


    // Enter pressed from main menu
    selectVal = digitalRead(enterButton);
    if(selectVal==0){
      while(digitalRead(enterButton)==0){ // wait until let go of button
        delay(10);
      }

      // Process enter
      switch (curMenuItem){
        case setStart:
            cDisplay();
            display.println("Starting..");
            display.setTextSize(1);
            display.display();
            delay(2000);
            startRec = 1;  //start recording 
            break;
        case setSchedule:
            while(digitalRead(enterButton)==1){
              int startTimeIndex;
              int endTimeIndex;
              if(digitalRead(downButton)==0) {
                startTimeIndex--;
                if(startTimeIndex<0) startTimeIndex = 0;
                while(digitalRead(downButton)==0); // wait to let go
              }
              if(digitalRead(upButton)==0) {
                startTimeIndex++;
                if(startTimeIndex>nTimes - 4) startTimeIndex = nTimes - 4;
                if(startTimeIndex<0) startTimeIndex = 0;
                while(digitalRead(upButton)==0); // wait to let go
              }

              endTimeIndex = startTimeIndex + 5;
              if(endTimeIndex>nTimes) endTimeIndex = nTimes;
              
              cDisplay();
              display.setTextSize(2);
              display.println("Schedule");
              display.setTextSize(1);
              
              for(int i=startTimeIndex; i<endTimeIndex; i++){
                display.print(i); display.print(":");
                printZero(scheduleHour[i]);
                display.print(scheduleHour[i]); display.print(":");
                printZero(scheduleMinute[i]);
                display.print(scheduleMinute[i]); display.print(" ");
                display.print(duration[i]);
                display.println("m");
              }
              displayVoltage();
              display.display();
              delay(2);
            }
            while(digitalRead(enterButton)==0); // wait to let go
            curMenuItem = setStart;
            break;
      }
      
      if (settingsChanged) {
        settingsChanged = 0;
        autoStartTime = getUnixTime();  //reset autoStartTime
      }
    }

    cDisplay();
    displayMenu();
    displayVoltage();
    displayClock(BOTTOM);
    display.display();
    delay(10);
  }
}

void cDisplay(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0,0);
}

void displayClock(int loc){
  getTime();  // microcontroller clock
  display.setTextSize(1);
  display.setCursor(0,loc);
  display.print(year);
  display.print('-');
  display.print(month);
  display.print('-');
  display.print(day);
  display.print("  ");
  printZero(hour);
  display.print(hour);
  printDigits(minute);
  printDigits(second);
}

void printTime(){
  SerialUSB.print(year);
  SerialUSB.print('-');
  SerialUSB.print(month);
  SerialUSB.print('-');
  SerialUSB.print(day);
  SerialUSB.print(" ");
  SerialUSB.print(hour);
  SerialUSB.print(':');
  SerialUSB.print(minute);
  SerialUSB.print(':');
  SerialUSB.println(second);
}

void displayMenu(){
  display.setTextSize(2);
  display.println(menuItem[curMenuItem]);
  display.setTextSize(1);
  display.println(helpText[curMenuItem]);
}

void displayVoltage(){
  display.setTextSize(1);
  display.setCursor(100, 0);
  display.print(readVoltage(),1);
  display.print("V");
}
