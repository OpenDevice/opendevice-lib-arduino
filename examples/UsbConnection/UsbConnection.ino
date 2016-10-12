/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/Getting+started
 * This example code is in the public domain.
 * *****************************************************************************
 */
#include <OpenDevice.h>

void setup(){
    ODev.enableDebug();
    ODev.addDevice("LED1", 13, Device::DIGITAL); // ID:1
    ODev.begin(); // by default call Serial.begin() and while(!Serial) on Leonardo
}

void loop(){
	ODev.loop();
}

