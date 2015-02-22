/*
 * ******************************************************************************
 *  Copyright (c) 2013-2014 CriativaSoft (www.criativasoft.com.br)
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *  Ricardo JL Rufino - Initial API and Implementation
 * *****************************************************************************
 */


#include "OpenDevice.h"

/*
 * OpenDeviceClass.cpp
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */
OpenDeviceClass::OpenDeviceClass() {
	deviceConnection = NULL;
	autoControl = false;
	debugMode = false;
	keepAliveEnabled = false;
	keepAliveTime = 0;
	debugTarget = 0; // 0 - Default Serial, 1 - Target Connection
	time = 0;
	deviceLength = 0;
	commandsLength = 0;
	for (int i = 0; i < MAX_DEVICE; i++) {
		devices[i] = NULL;
	}
}

byte OpenDeviceClass::mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};  // TODO: read from EPROM.

//void OpenDeviceClass::begin(Stream &serial) {
//	OpenDeviceClass::begin(&DeviceConnection(serial));
//}

//#if defined(ethernet_h)
//
//void OpenDeviceClass::begin() {
//	  // Start the Ethernet connection and the server
//	  if (Ethernet.begin(mac) == 0) {
//	    debug("Failed to configure Ethernet using DHCP");
//	    // no point in carrying on, so do nothing forevermore:
//	    // try to congifure using IP address instead of DHCP:
//	    Ethernet.begin(mac, ip);
//	  }
//}
//
//#else
void OpenDeviceClass::_begin() {
	begin(Serial, DEFAULT_BAUD);
}
//#endif

void OpenDeviceClass::begin(unsigned long baud) {
	#if(ENABLE_SERIAL)
		begin(Serial, baud);
	#endif
}

void OpenDeviceClass::begin(HardwareSerial &serial, unsigned long baud){

	serial.begin(baud);

	// Wait serial if using Leonardo
	#ifdef __AVR_ATmega32U4__
		//if(serial == SERIAL_PORT_USBVIRTUAL){
			while (!Serial){delay(1);}
		//}
	#endif

	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);

}

void OpenDeviceClass::begin(Stream &serial, unsigned long baud){
	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);
}

void OpenDeviceClass::begin(DeviceConnection &_deviceConnection) {

	deviceConnection = &_deviceConnection;

	for (int i = 0; i < deviceLength; i++) {
		devices[i]->init();
	}

	if(deviceConnection){
		deviceConnection->setDefaultListener(&(OpenDeviceClass::onMessageReceived));
	}


	ODev.debug("Begin [OK]");

}


void OpenDeviceClass::_loop() {

	if(deviceConnection && deviceConnection->conn){
		deviceConnection->checkDataAvalible();
	}

	checkSensorsStatus();

	// Send PING/KeepAlive if enabled
	if(keepAliveEnabled){
	  unsigned long currentMillis = millis();
	  if(currentMillis - keepAliveTime > KEEP_ALIVE_INTERVAL) {
		keepAliveTime = currentMillis;
		ODev.send(cmd(CommandType::PING));
	  }
	}

}

void OpenDeviceClass::enableKeepAlive(){
	keepAliveEnabled = true;
}

void OpenDeviceClass::enableDebug(uint8_t _debugTarget){
	debugMode = true;
	debugTarget = _debugTarget;
}

/** Called when a command is received by the connection */
void OpenDeviceClass::onMessageReceived(Command cmd) {
	ODev.lastCMD = cmd;
	DeviceConnection *conn = ODev.deviceConnection;

	bool cont = true; // TODO: Chama handlers(functions), se retornar false abota a continuacao;

	// Directed to a device (Like On/OFF or more complex)
	if (cmd.deviceID > 0) {
		Device *foundDevice = ODev.getDevice(cmd.deviceID);
		if (foundDevice != NULL) {
			ODev.debugChange(foundDevice->id, cmd.value);
			foundDevice->setValue(cmd.value);
			foundDevice->deserializeExtraData(&cmd, conn);
			ODev.notifyReceived(ResponseStatus::SUCCESS);
		} else {
			ODev.notifyReceived(ResponseStatus::NOT_FOUND);
		}
	// User-defined command, this is an easy way to extend OpenDevice protocol.
	} else if (cmd.type == CommandType::USER_COMMAND) {
		String name = conn->readString();
		for (int i = 0; i < ODev.commandsLength; i++) {

			// if(ODev.debugMode){ ODev.debug("Call function:"); ODev.debug(name); }

			if (name.equals(ODev.commands[i].command)) {
				(*ODev.commands[i].function)();
			}
		}

		// Send response Ex: GET_DEVICES_RESPONSE;ID;[ID,PIN,VALUE,...];[ID,PIN,VALUE,...];[ID,PIN,VALUE,...]
	} else if (cmd.type == CommandType::GET_DEVICES) {

		conn->doStart();
		conn->print(CommandType::GET_DEVICES_RESPONSE);
		conn->doToken();
		conn->print(cmd.id);
		conn->doToken();

		for (int i = 0; i < ODev.deviceLength; ++i) {
			Device *device = ODev.getDeviceAt(i);
			// Write array to connection..
			char buffer[50]; // FIXME: daria para usar o mesmo buffer do deviceConnection ??
			device->toString(buffer);
			conn->print(buffer);

			if (i < ODev.deviceLength) {
				conn->doToken();
			}
		}

		conn->doEnd();

	}

}


// FIMXE: rename to onSensorChange
void OpenDeviceClass::onSensorChanged(uint8_t id, unsigned long value){
	Device* sensor = getDevice(id);
	Device* device = getDevice(sensor->targetID);

//    if(id < 100) // FIXME: detectar se é o IR.
//    	value = !value;        // NOTA: Os valores do Swicth sao invertidos

   ODev.debugChange(id, value);

	if(ODev.autoControl){
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
	Command lastCMD = ODev.lastCMD;
	lastCMD.id = 0;
	lastCMD.type = (uint8_t) sensor->type;
	lastCMD.deviceID = sensor->id;
	lastCMD.value = value;

	ODev.deviceConnection->send(lastCMD, false);
	// Check extra data to send.
	sensor->serializeExtraData(ODev.deviceConnection);
	ODev.deviceConnection->doEnd();
}


void OpenDeviceClass::send(Command cmd){
	if(deviceConnection){
		deviceConnection->send(cmd, true);
	}
}

Command OpenDeviceClass::cmd(uint8_t type, uint8_t deviceID, unsigned long value){
	Command cmd;
	cmd.type = type;
	cmd.deviceID = deviceID;
	cmd.value = value;
	return cmd;
}

void OpenDeviceClass::clear(Command cmd){
	lastCMD.id = 0;
	lastCMD.type = 0;
	lastCMD.deviceID = 0;
	lastCMD.value = 0;
	lastCMD.length = 0;
}


/** Send reply stating that the command was received successfully */
void OpenDeviceClass::notifyReceived(ResponseStatus::ResponseStatus status){
  // Serial.println("DB:notifyReceived");
  lastCMD.type = CommandType::DEVICE_COMMAND_RESPONSE;
  lastCMD.value = status;
  // FIXME: lastCMD.data =  NULL;
  lastCMD.length = 0;
  send(lastCMD);
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



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


bool OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID){
	bool v = addDevice(pin, type, true, 0);
	if(v) devices[deviceLength-1]->targetID = targetID;
	return v;
}

bool OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type){
	return addSensor(pin, type, -1);
}

bool OpenDeviceClass::addSensor(Device& sensor){
	return addDevice(sensor);
}


bool OpenDeviceClass::addDevice(uint8_t pin, Device::DeviceType type){
	return addDevice(pin, type, false, 0);
}


bool OpenDeviceClass::addDevice(Device& device){
	if (deviceLength < MAX_DEVICE) {

		if (device.sensor) {
			if (device.type == Device::DIGITAL) {
				pinMode(device.pin, INPUT_PULLUP); // Enable internal pull-up resistor..
			}
		} else {
			pinMode(device.pin, OUTPUT);
		}

		devices[deviceLength] = &device;
		deviceLength = deviceLength + 1;

		return true;
	} else{
		return false;
	}

}


bool OpenDeviceClass::addDevice(uint8_t pin, Device::DeviceType type, bool sensor, uint8_t id){
	if (deviceLength < MAX_DEVICE) {

		if (sensor) {
			if (type == Device::DIGITAL) {
				pinMode(pin, INPUT_PULLUP); // Enable internal pull-up resistor..
			}
		} else {
			pinMode(pin, OUTPUT);
		}

		if (id == 0) id = (deviceLength + 1);

		devices[deviceLength] = new Device(id, pin, type, sensor);
		deviceLength++;

		return true;
	} else{
		return false;
	}
}


bool OpenDeviceClass::addCommand(const char * name, void (*function)()){
	if (commandsLength < MAX_COMMAND) {

			strncpy(commands[commandsLength].command, name , MAX_COMMAND_STRLEN);
			commands[commandsLength].function = function;
			commandsLength++;

			return true;
		} else{
			return false;
		}
}


void OpenDeviceClass::checkSensorsStatus(){

	// Arduino DOC (http://arduino.cc/en/Reference/analogRead):
	// Takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times

	if(time == 0) time = millis();

	if (millis() - time > READING_INTERVAL){ // don't sample analog/digital more than XXXms
	    for (int i = 0; i < deviceLength; i++) {

	    	if(devices[i]->sensor && devices[i]->hasChanged()){
	    		onSensorChanged(devices[i]->id, devices[i]->currentValue);
	    	}

	    }

	    time = millis();
	}

}

void OpenDeviceClass::setValue(uint8_t id, unsigned long value){
    for (int i = 0; i < deviceLength; i++) {
    	if(devices[i]->id == id){
    		devices[i]->setValue(value);
    		break;
    	}
    }
}

void OpenDeviceClass::sendToAll(unsigned long value){
    for (int i = 0; i < deviceLength; i++) {
    	devices[i]->setValue(value);
    }
}

Device* OpenDeviceClass::getDevice(uint8_t id){

//	Serial.print("DB: getDevice :: size: ");
//	Serial.print(deviceLength, DEC);
//	Serial.print(" > ");

    for (int i = 0; i < deviceLength; i++) {

//    	Serial.print(devices[i]->id);
//    	Serial.print(",");

    	if(devices[i]->id == id){
//    		Serial.println("DB: getDevice TRUE");
//    		Serial.write(ACK_BIT);
    		return devices[i];
    	}
    }

//	Serial.println("DB: getDevice FALSE");
//	Serial.write(ACK_BIT);

    return NULL;
}

Device* OpenDeviceClass::getDeviceAt(uint8_t index){

	if(index > 0 && index <= deviceLength){
		return devices[index-1];
	}

    return NULL;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END: DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenDeviceClass::showFreeRam() {

  extern int __heap_start, *__brkval;
  int v;

  
  #if defined (E2END)
  Serial.print(F("DB:EPROM:"));
  Serial.print(E2END);
  Serial.print("-");
  #endif

  #if(ARDUINO)
  Serial.print(F("DB:RAM:"));
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
			Serial.write(Command::ACK_BIT);
			#endif
		}
	}
}


#ifdef ARDUINO
void OpenDeviceClass::debug(const String &str){
	if(debugMode){ // FIXME: a logica não está muito legal não... !
		if(debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:");
			deviceConnection->print(str);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			// FIXME: how to use another ports ? [Mega: 2 = Serial1, 3 = Serial2, 4 = Serial3 ]
			Serial.print("DB:");
			Serial.print(str);
			Serial.write(Command::ACK_BIT);
			#endif
		}
	}
}
#endif

OpenDeviceClass ODev;


