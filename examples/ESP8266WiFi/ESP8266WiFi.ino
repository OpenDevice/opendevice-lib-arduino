/**
 * The OpenDevice is compatible with ESP8266 in stand-alone mode or 
 * using the AT firmware. Enable correct dependency in file (OpenDevice/dependencies.h)
 * 
 * The Open Device will create a TCP server on the default port and a softAP using ODev.name("...").
 * 
 * 
 * Links:
 * - Tutorial: https://opendevice.atlassian.net/wiki/display/DOC/WiFi+using+ESP8266 
 * - https://github.com/esp8266/arduino
 * 
 * @author Ricardo JL Rufino
 * @date 16/nov/15
 */


#include <ESP8266WiFi.h>  // Enable ESP8266 Embedded
// #include <PubSubClient.h> // Enable + MQTT

#include <OpenDevice.h>

const char* ssid = "--";
const char* password = "--";

void setup() {
  ODev.name("ODev-Thing1");
  // Config.server = "192.168.3.105"; // Only for MQTT
  ODev.enableDebug();

  ODev.addDevice("LED", 2, Device::DIGITAL);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  ODev.begin(WiFi);
}

void loop() {
  ODev.loop();
}
