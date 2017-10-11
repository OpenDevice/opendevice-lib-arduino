/**
 * MQTT on YUN
 * @author Ricardo JL Rufino
 * @date 26/11/16
 */

#define ODEV_API_KEY "--------"
#define ODEV_MODULE_NAME "ODevYUN.1"

#include <YunClient.h>    // YUN
#include <PubSubClient.h> // MQTT

#include <OpenDevice.h>


void setup() {
  ODev.enableDebug();
  ODev.name(ODEV_MODULE_NAME);
  ODev.apiKey(ODEV_API_KEY);
  ODev.server("192.168.3.106"); // Change Server !

  ODev.addDevice("Yun1.LED", 13, Device::DIGITAL);

  ODev.begin();
}

void loop() {
  ODev.loop();
}
