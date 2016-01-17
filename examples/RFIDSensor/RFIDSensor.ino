/*
  This is a simple example of the integrated RFID Support.
  NOTE: Please enable MFRC522 dependency in "OpenDevice/dependencies.h"
  Library download: https://github.com/miguelbalboa/rfid
 */

#include <RCSwitch.h>
#include <OpenDevice.h>

RFSensor rf(0);

void setup(){
    ODev.enableDebug();
    ODev.enableKeepAlive(false);
    ODev.addSensor(rf); // ID:1
    ODev.begin();
}

void loop(){
  ODev.loop();
}


