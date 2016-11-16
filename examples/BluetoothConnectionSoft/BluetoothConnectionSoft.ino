//******************************************************************************
// See tutorial: 
// https://opendevice.atlassian.net/wiki/display/DOC/Getting+started
//******************************************************************************

 
#include <SoftSerial.h>   
#include <TinyPinChange.h>  
//#include <SoftwareSerial.h>

#include <OpenDevice.h>

//SoftSerial mySerial(2, 5); // RX, TX
SoftwareSerial mySerial(2, 5); // RX, TX

void setup(){
    // ODev.enableDebug();
    ODev.name("ODevBT");
    ODev.addDevice("LED", 13, Device::DIGITAL);
    
    mySerial.begin(9600);
    ODev.begin(mySerial); 
}

void loop(){
	ODev.loop();
}
