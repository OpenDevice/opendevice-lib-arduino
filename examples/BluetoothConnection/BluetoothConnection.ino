/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/A.+First+Steps+with+OpenDevice
 * This example code is in the public domain.
 * *****************************************************************************
 */
 
#include <OpenDevice.h>

void setup(){
    ODev.enableDebug();
    ODev.addDevice(13, Device::DIGITAL); // ID:1
    // ODev.begin(); // UNO
    ODev.begin(Serial1, 9600); //-- for Leonardo, Teensy (Rx:0,Tx:1)
}

void loop(){
	ODev.loop();
}