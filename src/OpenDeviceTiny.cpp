/*
 * OpenDeviceTiny.cpp
 *
 *  Created on: 21/07/2015
 *      Author: ricardo
 */

#include "OpenDeviceTiny.h"

OpenDeviceTiny::OpenDeviceTiny() {
	memset(devices, 0, MAX_DEVICE);
}

OpenDeviceTiny::~OpenDeviceTiny() {
	// TODO Auto-generated destructor stub
}

void OpenDeviceTiny::begin(DeviceConnection &_deviceConnection) {

	deviceConnection = &_deviceConnection;

//	for (int i = 0; i < deviceLength; i++) {
//		devices[i]->init();
//	}

	deviceConnection->setDefaultListener(&(OpenDeviceTiny::onMessageReceived));
	// ODev.debug("Begin [OK]");

}

void OpenDeviceTiny::_loop() {

	deviceConnection->checkDataAvalible();

//	checkSensorsStatus();
//

}

/** Called when a command is received by the connection */
void OpenDeviceTiny::onMessageReceived(Command cmd) {

	DeviceConnection *conn = ODevTiny.deviceConnection;

	bool cont = true; // TODO: Chama handlers(functions), se retornar false abota a continuacao;

	// Directed to a device (Like On/OFF or more complex)
	if (cmd.deviceID > 0) {
		uint8_t pin = ODevTiny.getDevice(cmd.deviceID);
		if (pin > 0) {
			// FIXME: allow analog...!!
			digitalWrite(pin, cmd.value);
			ODevTiny.notifyReceived(ResponseStatus::SUCCESS);
		} else {
			ODevTiny.notifyReceived(ResponseStatus::NOT_FOUND);
		}

	// Send response Ex: GET_DEVICES_RESPONSE;ID;[ID,PIN,VALUE,...];[ID,PIN,VALUE,...];[ID,PIN,VALUE,...]
	} else if (cmd.type == CommandType::GET_DEVICES) {

//		conn->doStart();
//		conn->print(CommandType::GET_DEVICES_RESPONSE);
//		conn->doToken();
//		conn->print(cmd.id);
//		conn->doToken();
//
//		char buffer[50] = {0};// FIXME: daria para usar o mesmo buffer do deviceConnection ??
//
//		for (int i = 0; i < ODev.deviceLength; ++i) {
//			Device *device = ODev.getDeviceAt(i);
//			// Write array to connection..
//			device->toString(buffer);
//			conn->print(buffer);
//
//			memset(buffer, 0, sizeof(buffer));
//
//			if (i < ODev.deviceLength) {
//				conn->doToken();
//			}
//		}
//
//		conn->doEnd();

	}

}

void OpenDeviceTiny::notifyReceived(ResponseStatus::ResponseStatus status){
  // Serial.println("DB:notifyReceived");
  Command lastCMD = deviceConnection->cmd;

  lastCMD.type = CommandType::DEVICE_COMMAND_RESPONSE;
  lastCMD.value = status;
  lastCMD.length = 0;

  deviceConnection->send(lastCMD, true);
}



bool OpenDeviceTiny::addDevice(uint8_t pin, Device::DeviceType type){
	if (deviceLength < MAX_DEVICE) {

		pinMode(pin, OUTPUT);

		uint8_t id = (deviceLength + 1);

		devices[deviceLength] = id;
		deviceLength++;

		return true;
	} else{
		return false;
	}
}


uint8_t OpenDeviceTiny::getDevice(uint8_t id){

	if(id < deviceLength){
		return devices[id];
	}else{
		return -1;
	}

}

OpenDeviceTiny ODevTiny;
