/*
 * *****************************************************************************
 * // TODO:  ADICIONAR DOCUMENTAÇO E LIN PARA O TUTORIAL...
 * This example code is in the public domain.
 * *****************************************************************************
 */


#include <OpenDevice.h>

// Connection Setup.
// Use Serial for USB and Serial1 for Bluetooth. Serial1 -> PC4=RX1,PC5=TX1 on Lauchpad
DeviceConnection deviceConnection(Serial);


void setup(){
  OpenDevice::debugMode = true;
  OpenDevice::debugTarget = 0;
  
  Serial.begin(9600);
  Serial1.begin(9600); // Bluetooth
  

  OpenDevice::debug("Starting...");

  OpenDevice::addDevice(RED_LED, Device::DIGITAL);   // ID:1
  OpenDevice::addDevice(GREEN_LED, Device::DIGITAL); // ID:2
  OpenDevice::addDevice(BLUE_LED, Device::DIGITAL);  // ID:3

  OpenDevice::addSensor(PUSH1, Device::DIGITAL); // ID:4
  OpenDevice::addSensor(PUSH2, Device::DIGITAL); // ID:5
			
  OpenDevice::begin(deviceConnection);			
  OpenDevice::debug("Started!");

}

void loop(){
  
  OpenDevice::loop();
 
}


