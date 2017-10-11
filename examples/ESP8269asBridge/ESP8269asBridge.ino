// ESP8266 as bridge
// Tutorial : https://opendevice.atlassian.net/wiki/display/DOC/ESP8266+as+bridge
//

#include <ELClient.h>
#include <OpenDevice.h>

#include <SoftwareSerial.h>
SoftwareSerial debugport(10,11); // RX, TX (Optional)
EspLinkConnection conn(Serial, debugport);

void setup() {
  // ODev.enableDebug();
  Serial.begin(115200); // ESP-Link port (change to correct port)
  debugport.begin(115200);
  while(!Serial){delay(10);}

  ODev.name("ESPLink1");
  ODev.apiKey("--------");
  ODev.addDevice("ESP1.LED1", 13, Device::DIGITAL);

  ODev.begin(conn);
}

void loop() {
  ODev.loop();
}

