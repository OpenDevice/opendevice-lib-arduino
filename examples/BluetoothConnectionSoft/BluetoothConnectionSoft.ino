/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/A.+First+Steps+with+OpenDevice
 * This example code is in the public domain.
 * *****************************************************************************
 */
 
#include <SoftSerial.h>   
#include <TinyPinChange.h>  
//#include <SoftwareSerial.h>

#include <OpenDevice.h>

//SoftSerial mySerial(2, 5); // RX, TX
SoftwareSerial mySerial(2, 5); // RX, TX

void setup(){
    // ODev.enableDebug();
    ODev.addDevice(13, Device::DIGITAL); // ID:1
    
    mySerial.begin(9600);
    ODev.begin(mySerial); 
}

void loop(){
	ODev.loop();
}
