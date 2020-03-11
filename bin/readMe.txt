To load the binary run from the command line:C:\Users\dmann\AppData\Local\Arduino15\packages\arduino\tools\bossac\1.7.0-arduino3/bossac.exe -i -d --port=COM10 -U true -i -e -w -v C:\w\Arduino_build/Metronome.ino.bin -R 

Things you will have to change:
1. Path for your Arduino installation where the binary uploader program is located (bossac.exe). This is dmann in example above.
2. Path to the binary Metronome.ino.bin3 downloaded from github. 
3. You will have to figure out what COM Port the board connected to. 
   The easiest way to do this is to look in the Arduino IDE Tools menu, select Port.
4. You may need to double tap on the little button below the microcontroller to get it into bootloader mode before running that command.