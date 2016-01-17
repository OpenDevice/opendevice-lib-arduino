/*
  This is a simple example of the integrated RFID Support.
  NOTE: Please enable MFRC522 dependency in "OpenDevice/dependencies.h"
  Library Used: https://github.com/miguelbalboa/rfid
 */

#include <MFRC522.h>
#include <SPI.h>

#include <OpenDevice.h>

RFIDSensor rfid(10, 9);

void setup(){
    ODev.enableDebug();
    ODev.addSensor(rfid); // ID:1
    ODev.begin();
}

void loop(){
  ODev.loop();
}
