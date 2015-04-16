/*
 * *****************************************************************************
 * See tutorial: https://ODev.atlassian.net/wiki/display/DOC/A.+First+Steps+with+ODev
 * This example code is in the public domain.
 * *****************************************************************************
 * 
 */
//#include <SPI.h> 
#include <UIPEthernet.h>
#include <OpenDevice.h>

void setup(){

  Serial.begin(9600);
  Serial.print("Wait data !");
  while(!Serial.available()){ break;}

  // It's only required ENC28J60 ( UIPEthernet / arduino_uip  )
  #if defined (UIPETHERNET_H) && !UIP_CONF_UDP
  IPAddress myIP(192,168,2,106);
  Ethernet.begin(ODev.getMAC(A0), myIP);
  #endif
  
  ODev.enableDebug();
  ODev.addDevice(13, Device::DIGITAL);
  ODev.addCommand("mycmd1", ledON);
  ODev.addCommand("mycmd2", ledOFF);
  ODev.begin();
}

void loop(){
  ODev.loop();
}

// SEND: 99;0;mycmd1;MyString;10
void ledON(){
  ODev.debug(ODev.readString());
  int count = ODev.readInt();
  for (int i = 0; i < count; i++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
  digitalWrite(13, HIGH);
}

// SEND: 99;0;mycmd2
void ledOFF(){
  digitalWrite(13, LOW);
}
