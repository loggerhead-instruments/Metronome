int loadSchedule()
{
  char s[30];
  char c;
  unsigned int i = 0;
  int j = 0;

  File file;
  file = sd.open("schedule.txt", FILE_READ);
  SerialUSB.print("Schedule file: ");
  SerialUSB.println(file);
  if(file){
    do{
      int n;
      if(n = file.fgets(s, sizeof(s))){
        sscanf(&s[0],"%d:%d %d",&scheduleHour[i],&scheduleMinute[i],&duration[i]);
        SerialUSB.print(i); SerialUSB.print(" ");
        SerialUSB.print(scheduleHour[i]);SerialUSB.print(":");
        SerialUSB.print(scheduleMinute[i]);SerialUSB.print(" ");
        SerialUSB.println(duration[i]);
        i++;  // this gets rid of blank lines
        if(i==MAXTIMES) break;
      }
    }while(file.available());
    file.close();
    return i;  
  }
  
  cDisplay();
  display.println("Error");
  display.setTextSize(1);
  display.println("No schedule.txt");
  display.display();
  while(1);
  return 0;
}
