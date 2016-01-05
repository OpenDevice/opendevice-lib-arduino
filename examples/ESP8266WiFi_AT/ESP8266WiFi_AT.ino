/**
 * The OpenDevice is compatible with ESP8266 in stand-alone mode or 
 * using the AT firmware. Enable correct dependency in file (OpenDevice/dependencies.h)
 * 
 * The Open Device will create a TCP server on the default port and a softAP using ODev.name("...").
 * 
 * Links:
 * - Tutorial: https://opendevice.atlassian.net/wiki/display/DOC/WiFi+using+ESP8266 
 * - https://github.com/esp8266/arduino
 * 
 * @author Ricardo JL Rufino
 * @date 16/nov/15
 */


#include <ESP8266AT.h> // AT Firmware

#include <OpenDevice.h>
#include <utility/Timeout.h>

const char* ssid = "OpenWrt";
const char* password = "";

#define TARGET_SERIAL Serial1

void setup() {
  ODev.name("ODev-Thing1");
  ODev.enableKeepAlive(false);
  ODev.enableDebug();

  ODev.addDevice(2, Device::DIGITAL);

  WiFi.init(new ESP8266(TARGET_SERIAL, 115200)); // to: AT Firmware
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  ODev.begin(WiFi);
}

void loop() {

  // Send AT using console
  if(Serial.available()){
    readAndSendAT();
  }

  ODev.loop();
}

// Optional - Only for debug purpose
void readAndSendAT(){
    while(Serial.available() > 0){
      TARGET_SERIAL.write(Serial.read());
    }

    // wait for response
    String rec = WiFi.ESP->recvString("OK", "ERROR", 10000);
    Serial.print(rec);
}


