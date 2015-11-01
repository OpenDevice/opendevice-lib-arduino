/*
  This is an example of how to interact with more complex sensors, 
  which are not natively supported in OpenDevice.
  NOTE: Support for RFID is already in native OpenDevice, see example: RFIDSensor
 */

#include <MFRC522.h>
#include <SPI.h>

#include <OpenDevice.h>

#define RST_PIN    9 
#define SS_PIN    10 

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned long rfidValue;

void setup(){
    ODev.enableDebug();
    ODev.enableKeepAlive(false);
    ODev.addSensor(readRfid); // ID:1

    SPI.begin();
    mfrc522.PCD_Init();
    ODev.begin();
}

void loop(){
  ODev.loop();
}

unsigned long readRfid(){
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return rfidValue;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return rfidValue;
  }

  if (mfrc522.uid.size == 4) {

    byte *raw = mfrc522.uid.uidByte;

    rfidValue = (unsigned long) (raw[3] << 24) | (raw[2] << 16) | (raw[1] << 8) | raw[0];
    mfrc522.PICC_HaltA(); // mark as 'READ'
    return rfidValue;
  }
}


