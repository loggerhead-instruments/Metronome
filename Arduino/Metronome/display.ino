#define setStart 0
#define setRecDur 1
#define setRecSleep 2
boolean settingsChanged = 0;
int curMenuItem = 0;
volatile int maxMenuItem = 3;
char *menuItem[] = {"Start",
                     "Channel",
                     "Start Hour1",
                     "Duration",
                     "Gain",
                     "Time",
                     "Mode",
                     "Diel Time"
                     };

char *helpText[] = {"ENTER:Start RecordingupButton/DN:scroll menu",
                    "ENTER:Set Record Dur\nupButton/DN:scroll menu",
                    "ENTER:Set Sleep Dur\nupButton/DN:scroll menu",
                    "ENTER:Set Sample RateupButton/DN:scroll menu",
                    "ENTER:Set Gain (dB)\nupButton/DN:scroll menu",
                    "ENTER:Set Date/Time\nupButton/DN:scroll menu",
                    "ENTER:Set Mode\nupButton/DN:scroll menu",
                    "ENTER:Set Diel Time\nupButton/DN:scroll menu"
                    };

/* DISPLAY FUNCTIONS
 *  
 */

void displayOn(){
  //display.ssd1306_command(SSD1306_DISPLAYON);
  display.init();
  display.setBatteryVisible(true);
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

  // Main Menu Loop
   while(startRec==0){

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
            display.print("Press upButton+DN to Stop");
            display.display();
            delay(2000);
            startRec = 1;  //start recording 
            break;
        case setRecDur:
            while(digitalRead(enterButton)==1){
              rec_dur = updateVal(rec_dur, 1, 3600);
              cDisplay();
              display.println("Record:");
              display.print(rec_dur);
              display.println("s");
              displayVoltage();
              display.display();
              delay(2);
            }
            while(digitalRead(enterButton)==0); // wait to let go
            curMenuItem = setStart;
            break;
          
        case setRecSleep:
          while(digitalRead(enterButton)==1){
            rec_int = updateVal(rec_int, 0, 3600 * 24);
            cDisplay();
            display.println("Sleep:");
            display.print(rec_int);
            display.println("s");
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
        // autoStartTime = getTeensy3Time();  //reset autoStartTime
      }
    }

    cDisplay();
    displayMenu();
    displayVoltage();
  //  displayClock(BOTTOM);
    display.display();
    delay(10);
  }
}
  
int updateVal(long curVal, long minVal, long maxVal){
  boolean upVal = digitalRead(upButton);
  boolean downVal = digitalRead(downButton);
  static int heldDown = 0;
  static int heldUp = 0;

  if(upVal==0){
    settingsChanged = 1;
    if (heldUp < 20) delay(200);
      curVal += 1;
      heldUp += 1;
    }
    else heldUp = 0;

    if (heldUp > 100) curVal += 4; //if held up for a while skip an additional 4
    if (heldUp > 200) curVal += 55; //if held up for a while skip an additional 4
    
    if(downVal==0){
      settingsChanged = 1;
      if(heldDown < 20) delay(200);
      if(curVal < 61) { // going down to 0, go back to slow mode
        heldDown = 0;
      }
        curVal -= 1;
        heldDown += 1;
    }
    else heldDown = 0;

    if(heldDown > 100) curVal -= 4;
    if(heldDown > 200) curVal -= 55;

    if (curVal < minVal) curVal = maxVal;
    if (curVal > maxVal) curVal = minVal;
    return curVal;
}

void cDisplay(){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,displayLine1);
}

void displayClock(int loc){
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
  Serial.print(year);
  Serial.print('-');
  Serial.print(month);
  Serial.print('-');
  Serial.print(day);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(':');
  Serial.print(minute);
  Serial.print(':');
  Serial.println(second);
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
