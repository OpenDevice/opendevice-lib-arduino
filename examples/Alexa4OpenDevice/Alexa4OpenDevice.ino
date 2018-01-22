/**
 * The OpenDevice and Alexa Integration (ESP8266 Only).
 * 
 * This example allows Alexa to natively recognize the devices configured by OpenDevice. 
 * Just say: Alexa, discovery devices | Alexa, turn ON the TV
 * For this the parameter must be enabled: ENABLE_ALEXA_PROTOCOL (in file: config.h)
 * You can still control them by the OpenDevice Platform.
 * 
 * SECURITY ALERT: Note that the device is becoming available on your local network allowing anyone 
 * with advanced knowledge to control them.
 *
 * Tutorial : https://ricardojlrufino.wordpress.com/2017/10/11/alexa-esp8266-opendevice/
 * 
 * @date 10/out/17
 */

#define ODEV_API_KEY "------"
#define ODEV_MODULE_NAME "Alexa4ODev-01"

#include <ESP8266WiFi.h>  // Enable ESP8266 Embedded
#include <PubSubClient.h> // MQTT
#include <ArduinoOTA.h>   // Remote Updates

#include <OpenDevice.h>


const char* ssid = "------";
const char* password = "-----";

void setup() {
  ODev.enableDebug();
  ODev.name(ODEV_MODULE_NAME);
  ODev.apiKey(ODEV_API_KEY);
  ODev.server("cloud.opendevice.io"); // Change Server !

  ODev.addDevice("Light", 2, Device::DIGITAL)->invertedState();
  ODev.addDevice("TV", 2, Device::DIGITAL)->invertedState();

  // NOTE: for security it is important to leave disabled
  Alexa.enableDiscovery();
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  ODev.begin();
 
}

void loop() {
  ODev.loop();
}
