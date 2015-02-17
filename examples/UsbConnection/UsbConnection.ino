/*
 * *****************************************************************************
 * See tutorial: https://opendevice.atlassian.net/wiki/display/DOC/A.+First+Steps+with+OpenDevice
 * This example code is in the public domain.
 * *****************************************************************************
 */

#include <DeviceConnection.h>
#include <OpenDevice.h>

void setup(){
    OpenDevice.debugMode = true;
    Serial.begin(9600);  
    while (!Serial){delay(1);} // uncomment if using Leonardo
   
    OpenDevice.addDevice(13, Device::DIGITAL); // ID:1
    
    OpenDevice.begin(Serial);
    OpenDevice.debug("Started!");
         
}

void loop(){
	
	OpenDevice.loop();

}

