/*
 * OpenDeviceClass.cpp
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */

#include "OpenDevice.h"

OpenDeviceClass::OpenDeviceClass() {
	deviceManager = new DeviceManager();
	deviceConnection = NULL;
	autoControl = false;
	debugMode = false;
	debugTarget = 0; // 0 - Default Serial, 1 - Target Connection
}


//OpenDeviceClass::~OpenDeviceClass() {
//	// TODO Auto-generated destructor stub
//}

//void OpenDeviceClass::begin(Stream &serial) {
//	OpenDeviceClass::begin(&DeviceConnection(serial));
//}

void OpenDeviceClass::begin(unsigned long baud) {
#if(ENABLE_SERIAL)
	Serial.begin(baud);
	DeviceConnection *conn =  new DeviceConnection(Serial);
	begin(*conn);
#endif
}

void OpenDeviceClass::begin(DeviceConnection &_deviceConnection) {

	deviceConnection = &_deviceConnection;

	if(deviceManager){
		deviceManager->setDefaultListener(&(OpenDeviceClass::sensorChanged));
		deviceManager->init(); // Init Sensors and Devices.
	}

	if(deviceConnection){
		deviceConnection->setDefaultListener(&(OpenDeviceClass::onMessageReceived));
	}

}


bool OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID){
	return deviceManager->addSensor(pin, type, targetID);
}

bool OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type){
	return deviceManager->addSensor(pin, type, -1);
}

bool OpenDeviceClass::addDevice(uint8_t pin, Device::DeviceType type){
	return deviceManager->addDevice(pin, type);
}

bool OpenDeviceClass::addDevice(Device& device){
	return deviceManager->addDevice(device);
}

bool OpenDeviceClass::addSensor(Device& sensor){
	return deviceManager->addDevice(sensor);
}

void OpenDeviceClass::loop() {

	if(deviceConnection){
		deviceConnection->checkDataAvalible();
	}

	if(deviceManager){
		deviceManager->checkStatus();
	}

}

/** Called when a command is received by the connection */
void OpenDeviceClass::onMessageReceived(Command cmd){
	OpenDevice.lastCMD = cmd;
	DeviceManager *deviceManager = OpenDevice.deviceManager;
	DeviceConnection *deviceConnection = OpenDevice.deviceConnection;

	bool cont = true; // TODO: Chama handlers(functions), se retornar false abota a continuacao;

	// Send response Ex: GET_DEVICES_RESPONSE;ID;[ID,PIN,VALUE,...];[ID,PIN,VALUE,...];[ID,PIN,VALUE,...]
	if(cmd.type== CommandType::GET_DEVICES){

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

	}else if(cmd.deviceID > 0){

		Device *findDevice = deviceManager->getDevice(cmd.deviceID);
		if (findDevice != NULL) {
			OpenDevice.debugChange(findDevice->id, cmd.value);
			findDevice->setValue(cmd.value);
			findDevice->deserializeExtraData(&cmd, deviceConnection);
			OpenDevice.notifyReceived(ResponseStatus::SUCCESS);
		} else {
			OpenDevice.notifyReceived(ResponseStatus::NOT_FOUND);
		}

	}
}


void OpenDeviceClass::sensorChanged(uint8_t id, unsigned long value){
	Device* sensor = OpenDevice.deviceManager->getDevice(id);
	Device* device = OpenDevice.deviceManager->getDevice(sensor->targetID);

//    if(id < 100) // FIXME: detectar se é o IR.
//    	value = !value;        // NOTA: Os valores do Swicth sao invertidos

   OpenDevice.debugChange(id, value);

	if(OpenDevice.autoControl){
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

	// SEND: Command
	// ==========================
	Command lastCMD = OpenDevice.lastCMD;
	lastCMD.id = 0;
	lastCMD.type = (uint8_t) sensor->type;
	lastCMD.deviceID = sensor->id;
	lastCMD.value = value;

	// Check extra data to send.
	OpenDevice.deviceConnection->send(lastCMD, false);
	sensor->serializeExtraData(OpenDevice.deviceConnection);
	OpenDevice.deviceConnection->doEnd();
}


void OpenDeviceClass::sendCommand(Command cmd){
	if(deviceConnection){
		deviceConnection->send(cmd);
	}
}


/** Send reply stating that the command was received successfully */
void OpenDeviceClass::notifyReceived(ResponseStatus::ResponseStatus status){
  // Serial.println("DB:notifyReceived");
  lastCMD.type = CommandType::DEVICE_COMMAND_RESPONSE;
  lastCMD.value = status;
  // FIXME: lastCMD.data =  NULL;
  lastCMD.length = 0;
  sendCommand(lastCMD);
}

void OpenDeviceClass::debugChange(uint8_t id, unsigned long value){

	if(debugMode){

		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:CHANGE:");
			deviceConnection->print(id);
			deviceConnection->print("=");
			deviceConnection->print(value);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:CHANGE:");
			Serial.print(id);
			Serial.print("=");
			Serial.println(value);
			deviceConnection->doEnd(); // FIXME: isso nao era pra estar aqui
                                       // o problema é que quando é USB a connexão princial..
			#endif
		}
	}
}

void OpenDeviceClass::freeRam() {

  extern int __heap_start, *__brkval;
  int v;

  
  #if defined (E2END)
  Serial.print("DB:EPROM:");
  Serial.print(E2END);
  Serial.print("-");
  #endif

  #if(ARDUINO)
  Serial.print("DB:RAM:");
  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
  #endif
}


void OpenDeviceClass::debug(const char str[]){
	if(debugMode){ // FIXME: a logica não está muito legal não... !
		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:");
			deviceConnection->print(str);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:");
			Serial.print(str);
			Serial.write(19);
			#endif
		}
	}
}

#ifdef ARDUINO
void OpenDeviceClass::debug(String& str){
	if(debugMode){ // FIXME: a logica não está muito legal não... !
		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:");
			deviceConnection->print(str);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:");
			Serial.print(str);
			Serial.write(19);
			#endif
		}
	}
}
#endif

OpenDeviceClass OpenDevice;


