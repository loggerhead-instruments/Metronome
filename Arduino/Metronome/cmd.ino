int loadSchedule()
{
  char s[30];
  char c;
  short i = 0;
  int j = 0;

  File file;

  delay(1000);

  // Read simulated depth file
  file = sd.open("schedule.txt", FILE_READ);
  SerialUSB.print("Schedule file: ");
  SerialUSB.println(file);
  delay(1000);
  if(file){
    do{
      j = 0;
      do{ // scan next line
        c = file.read();
        SerialUSB.write(c);
        if(c!='\r') s[j] = c;
        j++;
        if(j>29) break;
      }while(c!='\n');

      sscanf(&s[0],"%d:%d %d",&scheduleHour[i],&scheduleMinute[i],&duration[i]);
      SerialUSB.print(i); SerialUSB.print(" ");
      SerialUSB.print(scheduleHour[i]);SerialUSB.print(":");
      SerialUSB.print(scheduleMinute[i]);SerialUSB.print(" ");
      SerialUSB.println(duration[i]);
      i++;
      if(i==MAXTIMES) break;
    }while(file.available());
    file.close();
    return i;  
  }
  return 0;
}
