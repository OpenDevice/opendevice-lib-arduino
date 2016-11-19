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


#define ODEV_API_KEY ""
#define ODEV_MODULE_NAME "ODevESP8266-1"

#include <ESP8266WiFi.h>  // Enable ESP8266 Embedded
#include <PubSubClient.h> // MQTT
#include <ArduinoOTA.h>   // Remote Updates

#include <OpenDevice.h>


const char* ssid = "--";
const char* password = "--";

void setup() {
  ODev.enableDebug();
  ODev.name(ODEV_MODULE_NAME);
  ODev.apiKey(ODEV_API_KEY);
  ODev.server("192.168.3.106"); // Change Server !

  ODev.addDevice("LED", 2, Device::DIGITAL);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  ODev.begin();
}

void loop() {
  ODev.loop();
}
