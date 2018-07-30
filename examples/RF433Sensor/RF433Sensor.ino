/*
 *  OpenDevice RF(sensors) native support
 *  Dependency: https://github.com/sui77/rc-switch 
 *  Tutorial: 
 *  References: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
 */

#include <RCSwitch.h>

//#include <PubSubClient.h> 
//#include <ESP8266WiFi.h> // Standalone
//#include <ArduinoOTA.h>

#include <OpenDevice.h>

RFSensor rfSensor(D5); // PIN must support interrupts

void setup() {
  ODev.enableDebug();
  ODev.addSensor("Q1.rfSensor", rfSensor)->onChange(rfReceived);
  rfSensor.filterDuplicatedInterval(3000);
  ODev.begin();
}

void loop() {
  ODev.loop();
}

/**
 * Logic and command mapping can be done on the server using Rules. 
 * If you need to do some more complex logic internally, you can use this method.
 */
bool rfReceived(uint8_t id, value_t value){
  if(value == 141831349){
    Serial.print("Off-Line Logic !!");
  }
  return true;
}
