/**
 * OpenDevice / SonOff Integration
 * 
 * This example demonstrates the configuration of a device that has the characteristic 
 * of being mobile (entering several Wifi zones)
 * It was designed to work primarily on modules called SonOff, but can be adapted to 
 * any device based on Esp8266
 * 
 * Tutorial: https://opendevice.atlassian.net/wiki/spaces/DOC/pages/318472214/SonOff+Integration
 * 
 * @date 07/jun/18 (Ricardo JL Rufino)
 */ 

#include <ESP8266WiFi.h>  // Enable ESP8266 Embedded
#include <PubSubClient.h> // MQTT
#include <ArduinoOTA.h>   // Remote Updates

#include <OpenDevice.h>

// Enable Wifi Setup / AutoConnect
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <utility/ODevWiFi.h>

#define LED_PIN 13 // For SonOff
#define RELAY_PIN 12 // For SonOff 
#define RESET_PIN 0 // For SonOff

//#define LED_PIN D4 // For ESP12E
//#define RELAY_PIN D7 // For ESP12E
//#define RESET_PIN D3 // For ESP12E

void setup() {

  ODev.name(Config.moduleName); // use saved name
  ODev.enableDebug();
  ODev.resetPin(RESET_PIN);
  
  // -------------- WiFi Setup ---------------//

  od::WiFi wifi;
  wifi.setLed(LED_PIN, /*OFF on:*/ HIGH);
  wifi.enableAutoSetup(false); // GoTo setup mode ONLY if RESET pressed
  wifi.enableServerParams(true); // Allow customize Server, ApiKey, DeviceName
  wifi.autoConnect(/*AP Passwd:*/"12345678");

  // -------------- ODev Setup ---------------//
 
  ODev.addDevice("Switch",  RELAY_PIN, Device::DIGITAL)->onChange(onSwitchChange);
  
  ODev.begin();
}

// Change Led 
bool onSwitchChange(uint8_t id, value_t value) {

  if(value > 0) digitalWrite(LED_PIN, LOW);
  else digitalWrite(LED_PIN, HIGH);

  return true;
}

void loop() {  
  ODev.loop();
}
