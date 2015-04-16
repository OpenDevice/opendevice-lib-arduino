/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/A.+First+Steps+with+OpenDevice
 * This example code is in the public domain.
 * *****************************************************************************
 */
 
#include <SoftwareSerial.h>
#include <OpenDevice.h>

void setup(){
    ODev.enableDebug();
    ODev.addDevice(13, Device::DIGITAL); // ID:1
    ODev.begin(9600, 10, 11); 
}

void loop(){
	ODev.loop();
}