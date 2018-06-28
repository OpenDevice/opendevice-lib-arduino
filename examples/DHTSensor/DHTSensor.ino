// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
//
// Tutorial:
// https://opendevice.atlassian.net/wiki/spaces/DOC/pages/11337730/Connecting+Devices+using+MQTT

#include <ESP8266WiFi.h>  // Enable ESP8266 Embedded
#include <PubSubClient.h> // MQTT
#include <ArduinoOTA.h>   // Remote Updates

// Sensor Libraries
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <OpenDevice.h> // needs to be the last

#define DHTPIN            D2         // Pin which is connected to the DHT sensor.

// Available types:  DHT11, DHT22, DHT21
DHT_Unified dht(DHTPIN, DHT11);

DHT_Unified::Temperature sTemp = dht.temperature();
DHT_Unified::Humidity sHumidity = dht.humidity();

void setup() {
  Serial.begin(115200);

  ODev.name("ODevHT-01");
  ODev.apiKey("----APIKEY-----");
  ODev.server("----SERVER IP-----");

  dht.begin(); // Initialize device.

  ODev.addSensor("HT01_Temperature", new AdafruitSensor(sTemp))
    ->setInterval(2 * (1000)); // 2sec

  ODev.addSensor("HT01_Humidity", new AdafruitSensor(sHumidity))
    ->setInterval(2 * (1000)); // 2sec

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin("---- WIFI----", "---- PASS----");

  ODev.begin();
}

void loop() {
  ODev.loop();
}
