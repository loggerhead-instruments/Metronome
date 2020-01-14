#define PMTK_LOCUS_STARTLOG  "$PMTK185,0*22"
#define PMTK_LOCUS_STOPLOG "$PMTK185,1*23"
#define PMTK_LOCUS_STARTSTOPACK "$PMTK001,185,3*3C"
#define PMTK_LOCUS_QUERY_STATUS "$PMTK183*38"
#define PMTK_LOCUS_ERASE_FLASH "$PMTK184,1*22"
#define PMTK_LOCUS_DUMP "$PMTK622,1*29"
// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF "$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"

#define maxChar 256
char gpsStream[maxChar];
int streamPos;
volatile boolean endGpsLog;


int gps(byte incomingByte){
  char temp2[2];
  char temp3[3];
  char temp5[5];
  char temp7[7];
  char temp12[12];
  // check for start of new message
  // if a $, start it at Pos 0, and continue until next G
  if(incomingByte=='$') {
   // SerialUSB.print("String position:");
   // SerialUSB.println(streamPos);
   // SerialUSB.println(gpsStream);
    //process last message
    if(streamPos > 10){
      // OriginGPS
      // $GNRMC,134211.000,A,2715.5428,N,08228.7924,W,1.91,167.64,020816,,,A*62
      // Adafruit GPS
      // $GPRMC,222250.000,A,2716.6201,N,08227.4996,W,1.01,301.49,250117,,,A*7C
      char rmcCode[6 + 1];
      float rmcTime; //           225446       Time of fix 22:54:46 UTC
      char rmcValid[2]; //           A            Navigation receiver warning A = OK, V = warning
      float rmcLat; //           4916.45,N    Latitude 49 deg. 16.45 min North
      char rmcLatHem[2];
      float rmcLon; //           12311.12,W   Longitude 123 deg. 11.12 min West
      char rmcLonHem[2];
      float rmcSpeed; //           000.5        Speed over ground, Knots
      float rmcCourse;//           054.7        Course Made Good, True
      char rmcDate[6 + 1];//           191194       Date of fix  19 November 1994
      float rmcMag;//           020.3,E      Magnetic variation 20.3 deg East
      char rmcMagHem[2];
      char rmcChecksum[4 + 1]; //           *68          mandatory checksum

      // check for end of log dump  $PMTKLOX,2*47
      if(gpsStream[1]=='P' & gpsStream[2]=='M' &  gpsStream[3]=='T' &  gpsStream[4]=='K' &  gpsStream[5]=='L'  
      & gpsStream[6]=='O' &  gpsStream[7]=='X' &  gpsStream[8]==',' &  gpsStream[9]=='2' & gpsStream[10]=='*' 
      & gpsStream[11]=='4' & gpsStream[12]=='7'){
        endGpsLog = 1;
      }

      if(gpsStream[1]=='G' & gpsStream[2]=='P' &  gpsStream[3]=='R' &  gpsStream[4]=='M' &  gpsStream[5]=='C'){
         char temp[streamPos + 1];
         //char temp[100];
         const char s[2] = ",";
         char *token;

         char splitStr[13][15];
         int j = 0;
         int k = 0;
         // parse GPS Stream
         for(int i=0; i<streamPos; i++){
          //SerialUSB.print("NextVal:");
          //SerialUSB.println(gpsStream[i]);
          if(gpsStream[i]!=',') 
            splitStr[j][k] = gpsStream[i];
          else{
            splitStr[j][k] = '\0';
//            SerialUSB.print(j); SerialUSB.print(":");
//            SerialUSB.println(splitStr[j]);
            k = -1;  // so ends up being 0 after k++
            j++;
          }
          k++;
         }
         splitStr[j][k] = '\0'; // terminate last one

         sscanf(splitStr[1], "%2d%2d%2d", &gpsHour, &gpsMinute, &gpsSecond); 
         sscanf(splitStr[2], "%s", rmcValid);  
         String rmcLatStr = String(splitStr[3]);
         rmcLat = rmcLatStr.toFloat();
         sscanf(splitStr[4], "%s", rmcLatHem);
         String rmcLonStr = String(splitStr[5]);
         rmcLon = rmcLonStr.toFloat(); 
         sscanf(splitStr[6], "%s", rmcLonHem);
         sscanf(splitStr[9], "%2d%2d%2d", &gpsDay, &gpsMonth, &gpsYear);

//         SerialUSB.print("rmcLat:"); SerialUSB.println(rmcLat, 6);
//         SerialUSB.print("rmcLon:"); SerialUSB.println(rmcLon, 6);

         float tempLatitude, tempLongitude;
         if(rmcValid[0]=='A'){
           tempLatitude = rmcLat;
           tempLongitude = rmcLon;
           latHem = rmcLatHem[0];
           lonHem = rmcLonHem[0];
           if(latHem=='S') tempLatitude = -tempLatitude;
           if(lonHem=='W') tempLongitude = -tempLongitude;
           latitude = convertDegMinToDecDeg(tempLatitude);
           longitude = convertDegMinToDecDeg(tempLongitude);
           goodGPS = 1;
//           SerialUSB.print("Temp Lat:"); SerialUSB.println(tempLatitude);
//           SerialUSB.print("Temp Lon:"); SerialUSB.println(tempLongitude);
//           SerialUSB.print("Lat:"); SerialUSB.println(latitude);
//           SerialUSB.print("Lon:"); SerialUSB.println(longitude);
        }
      }
    }
    // start new message here
    streamPos = 0;
  }
  gpsStream[streamPos] = incomingByte;
  streamPos++;
  if(streamPos >= maxChar) streamPos = 0;
}

void gpsStartLogger(){
  gpsSerial.println(PMTK_LOCUS_STARTLOG);
  waitForGPS();
}

void gpsStopLogger(){
  gpsSerial.println(PMTK_LOCUS_STOPLOG);
  waitForGPS();
}

void gpsEraseLogger(){
  gpsSerial.println(PMTK_LOCUS_ERASE_FLASH);
  waitForGPS();
}

void gpsStatusLogger(){
  gpsSerial.println(PMTK_LOCUS_QUERY_STATUS);
  waitForGPS();
}

void gpsSleep(){
  gpsSerial.println("$PMTK161,0*28");
  gpsSerial.flush();
}

void gpsHibernate(){
  gpsSerial.println("$PMTK225,4*2F");
  gpsSerial.flush();
}

void gpsWake(){
  gpsSerial.println(".");
  gpsSerial.flush();
}

void gpsSpewOff(){
  gpsSerial.println(PMTK_SET_NMEA_OUTPUT_OFF);
}

void gpsSpewOn(){
  gpsSerial.println(PMTK_SET_NMEA_OUTPUT_RMCONLY);
}

void waitForGPS(){
  for(int n=0; n<100; n++){
    delay(20);
    while (gpsSerial.available() > 0) {    
        byte incomingByte = gpsSerial.read();
        SerialUSB.write(incomingByte);
    }
  }
}

//int gpsDumpLogger(){
//  // open file for storing data; append
//  endGpsLog = 0;
//   gpsSerial.println(PMTK_LOCUS_DUMP);
//   int dumping = 1;
//   while(endGpsLog==0){
//       while (gpsSerial.available() > 0) {    
//        byte incomingByte = gpsSerial.read();
//        gps(incomingByte);
//        SerialUSB.write(incomingByte);
//       }
//    if(gpsTimeout >= gpsTimeOutThreshold) return 0;
//   }
//   return 1;
//}

double convertDegMinToDecDeg(float degMin) {
  double minDeg = 0.0;
  double decDeg = 0.0;
 
  //get the minutes, fmod() requires double
  minDeg = fmod((double)degMin, 100.0);
 
  //rebuild coordinates in decimal degrees
  degMin = (int) ( degMin / 100 );
  decDeg = degMin + ( minDeg / 60 );
 
  return decDeg;
}

void gpsGetTimeLatLon(){
    // get GPS
  int incomingByte;
  long gpsTimeOutStart = millis();

  if(introPeriod){
    cDisplay();
    display.println("GPS");
    display.println();
    display.setTextSize(1);
    display.println("Searching...");
    display.display();
  }

  goodGPS = 0;
  digitalWrite(gpsEnable, HIGH);
  gpsSerial.begin(9600);
  SerialUSB.println("GPS SerialUSB On");
  delay(100);
  gpsSpewOn();
  SerialUSB.println("Spew On");
  
  // can't display GPS data here, because display slows things down too much
  while((!goodGPS) & (millis()-gpsTimeOutStart<gpsTimeOutThreshold)){
    while (gpsSerial.available() > 0) {    
        incomingByte = gpsSerial.read();
        SerialUSB.write(incomingByte);
        gps(incomingByte);  // parse incoming GPS data
    }
  }

  SerialUSB.print("GPS search time:");
  SerialUSB.println(millis()-gpsTimeOutStart);
  gpsSpewOff();
  gpsSerial.end();
  digitalWrite(gpsEnable, LOW);
  SerialUSB.print("Good GPS:");
  SerialUSB.println(goodGPS);
}
