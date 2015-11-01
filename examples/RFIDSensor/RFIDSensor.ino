/*
  This is a simple example of the integrated RFID Support.
  NOTE: Please enable MFRC522 dependency in "OpenDevice/dependencies.h"
 */

#include <MFRC522.h>
#include <SPI.h>

#include <OpenDevice.h>

RFIDSensor rfid(10, 9);

void setup(){
    ODev.enableDebug();
    ODev.enableKeepAlive(false);
    ODev.addSensor(RFIDSensor(10,9)); // ID:1
    // ODev.begin(Serial1, 9600);
}

void loop(){
  ODev.loop();
}


