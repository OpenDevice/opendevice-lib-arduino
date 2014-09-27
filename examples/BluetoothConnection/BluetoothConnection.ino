/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/Arduino+Blink+Demo
 * This example code is in the public domain.
 * *****************************************************************************
 */


#include <DeviceConnection.h>
#include <OpenDevice.h>
#include <SoftwareSerial.h>

SoftwareSerial serialBT(8, 7); // RX  TX
DeviceConnection deviceConnection(serialBT);


void setup(){
    OpenDevice::debugMode = true;
    OpenDevice::debugTarget = 0; // 0:Serial, 1:DeviceConnection

    serialBT.begin(9600);
    Serial.begin(9600);
    while (!Serial){delay(1);} // uncomment if using Leonardo

    OpenDevice::addDevice(13, Device::DIGITAL); // ID:1
    
    OpenDevice::begin(deviceConnection);
    OpenDevice::debug("Started!");
         
}

void loop(){
	
	OpenDevice::loop();

}

