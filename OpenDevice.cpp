/*
 * OpenDevice.cpp
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */

#include "OpenDevice.h"

// Static member initialization...
DeviceConnection* OpenDevice::deviceConnection = NULL;
DeviceManager* OpenDevice::deviceManager = new DeviceManager();

Command OpenDevice::lastCMD;
bool OpenDevice::autoControl = false;

bool OpenDevice::debugMode = false;
uint8_t	OpenDevice::debugTarget = 1; // 0 - Default Serial, 1 - Target Connection

OpenDevice::OpenDevice() {
}


//OpenDevice::~OpenDevice() {
//	// TODO Auto-generated destructor stub
//}

//void OpenDevice::begin(Stream &serial) {
//	OpenDevice::begin(&DeviceConnection(serial));
//}

void OpenDevice::begin(DeviceConnection &_deviceConnection) {

	deviceConnection = &_deviceConnection;

	if(deviceManager){
		deviceManager->setDefaultListener(&(OpenDevice::sensorChanged));
		deviceManager->init(); // Init Sensors and Devices.
	}

	if(deviceConnection){
		deviceConnection->setDefaultListener(&(OpenDevice::onMessageReceived));
	}

}

bool OpenDevice::addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID){
	return deviceManager->addSensor(pin, type, targetID);
}

bool OpenDevice::addSensor(uint8_t pin, Device::DeviceType type){
	return deviceManager->addSensor(pin, type, -1);
}

bool OpenDevice::addDevice(uint8_t pin, Device::DeviceType type){
	return deviceManager->addDevice(pin, type);
}

void OpenDevice::loop() {

	if(deviceConnection){
		deviceConnection->checkDataAvalible();
	}

	if(deviceManager){
		deviceManager->checkStatus();
	}

}

void OpenDevice::onMessageReceived(Command cmd){
	OpenDevice::lastCMD = cmd;

	bool cont = true; // TODO: Chama handlers(functions), se retornar false abota a continuacao;

	if(!cont) return;

	if(cmd.type== CommandType::ON_OFF){ // TODO:MUDAR NOME? Device CONTROL. ?

		Device *findDevice = deviceManager->getDevice(cmd.deviceID);

		if (findDevice != NULL) {
    		debugChange(findDevice->id, cmd.value);
    		findDevice->setValue(cmd.value);
			notifyReceived(ResponseStatus::SUCCESS);
		} else {
			notifyReceived(ResponseStatus::NOT_FOUND);
		}


	// Send response Ex: GET_DEVICES_RESPONSE;ID;[ID,PIN,VALUE,...];[ID,PIN,VALUE,...];[ID,PIN,VALUE,...]
	}else if(cmd.type== CommandType::GET_DEVICES){

		deviceConnection->doStart();
		deviceConnection->print(CommandType::GET_DEVICES_RESPONSE);
		deviceConnection->doToken();
		deviceConnection->print(cmd.id);
		deviceConnection->doToken();

		for (int i = 0; i < deviceManager->deviceLength; ++i) {
			Device *device = deviceManager->getDeviceAt(i);
			// Write array to connection..
			char buffer[50]; // FIXME: daria para usar o mesmo buffer do deviceConnection ??
			device->toString(buffer);
			deviceConnection->print(buffer);

			if(i < deviceManager->deviceLength){
				deviceConnection->doToken();
			}

		}

		deviceConnection->doEnd();

	}
}


void OpenDevice::sensorChanged(uint8_t id, uint16_t value){
	Device* sensor = deviceManager->getDevice(id);
	Device* device = deviceManager->getDevice(sensor->targetID);

//    if(id < 100) // FIXME: detectar se é o IR.
//    	value = !value;        // NOTA: Os valores do Swicth sao invertidos

   debugChange(id, value);

	if(autoControl){
		if(device != NULL){
			// Sepre que uma alteracao for detectada, será invertido o seu estado atual.
			// Caso o dispositivo seja digital, ele ira reconhecer apenas dois valores, 0..1
			if(device->type == Device::DIGITAL){
				device->setValue( ! device->getValue() );
			}else{
				long cval = device->getValue();
				if(cval == 0) device->setValue(Device::MAX_ANALOG_VALUE); // set max
				else device->setValue(0); // set max
			}
		}
	}

	// SEND: ANALOG_REPORT
	// ==========================
	lastCMD.id = 0;
	lastCMD.type = (sensor->type == Device::DIGITAL ? CommandType::ON_OFF : CommandType::ANALOG_REPORT);
	lastCMD.deviceID = sensor->id;
	lastCMD.value = value;
	sendCommand(lastCMD);
}


void OpenDevice::sendCommand(Command cmd){
	if(deviceConnection){
		deviceConnection->send(cmd);
	}
}


/** Send reply stating that the command was received successfully */
void OpenDevice::notifyReceived(ResponseStatus::ResponseStatus status){
  // Serial.println("DB:notifyReceived");
  lastCMD.type = CommandType::DEVICE_COMMAND_RESPONSE;
  lastCMD.value = status;
  lastCMD.data =  NULL;
  lastCMD.length = 0;
  sendCommand(lastCMD);
}

void OpenDevice::debugChange(uint8_t id, uint16_t value){

	if(debugMode){

		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:CHANGE:");
			deviceConnection->print(id);
			deviceConnection->print("=");
			deviceConnection->print(value);
			deviceConnection->doEnd();
		}else{
			Serial.print("DB:CHANGE:");
			Serial.print(id);
			Serial.print("=");
			Serial.println(value);
			deviceConnection->doEnd(); // FIXME: isso nao era pra estar aqui
                                       // o problema é que quando é USB a connexão princial..

		}
	}
}

void OpenDevice::freeRam() {

  extern int __heap_start, *__brkval;
  int v;

  
  #if defined (E2END)
  Serial.print("DB:EPROM:");
  Serial.print(E2END);
  Serial.print("-");
  #endif

  Serial.print("DB:RAM:");
  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}


void OpenDevice::debug(const char str[]){
	if(debugMode){ // FIXME: a logica não está muito legal não... !
		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:");
			deviceConnection->print(str);
			deviceConnection->doEnd();
		}else{
			Serial.print("DB:");
			Serial.print(str);
			Serial.write(19);
		}
	}
}


