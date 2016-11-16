//******************************************************************************
// See tutorial: 
// https://opendevice.atlassian.net/wiki/display/DOC/Getting+started
//******************************************************************************

#include <OpenDevice.h>

void setup(){
	ODev.name("ODevBT");
    ODev.addDevice("LED", 13, Device::DIGITAL); 
    ODev.begin(Serial1, 9600); //-- for Leonardo, Teensy (Rx:0,Tx:1)
}

void loop(){
	ODev.loop();
}