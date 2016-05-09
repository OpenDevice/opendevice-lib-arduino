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

volatile uint8_t* PIN_INTERRUPT = 0;

/*
 * OpenDeviceClass.cpp
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */
OpenDeviceClass::OpenDeviceClass() {
	deviceConnection = NULL;
	autoControl = false;
	keepAliveEnabled = true;
	connected = false;
	keepAliveTime = 0;
	keepAliveMiss = 0;
	time = 0;
	deviceLength = 0;
	commandsLength = 0;
	for (int i = 0; i < MAX_DEVICE; i++) {
		devices[i] = NULL;
	}

	loadConfig();
}

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
void OpenDeviceClass::beginDefault() {
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

	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);

}

#if defined(HAVE_CDCSERIAL)
void OpenDeviceClass::begin(Serial_ &serial, unsigned long baud){

	serial.begin(baud);

	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);
}
#endif

#if defined (__arm__) && defined (__SAM3X8E__) // Arduino Due compatible
void OpenDeviceClass::begin(Serial_ &serial, unsigned long baud){

	serial.begin(baud);

	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);
}
#endif

#if defined(DEFAULT_TO_TINY_DEBUG_SERIAL)
void OpenDeviceClass::begin(TinyDebugSerial &serial, unsigned long baud){

//	serial.begin(baud);
//	DeviceConnection *conn =  new DeviceConnection(serial);
//	begin(*conn);
}
#endif

#if defined(DEFAULT_TO_TINY_DEBUG_SERIAL)
void OpenDeviceClass::begin(TinyDebugSerial &serial, unsigned long baud){

//	serial.begin(baud);
//	DeviceConnection *conn =  new DeviceConnection(serial);
//	begin(*conn);
}
#endif


void OpenDeviceClass::begin(Stream &serial){
	DeviceConnection *conn =  new DeviceConnection(serial);
	begin(*conn);
}

void OpenDeviceClass::begin(DeviceConnection &_deviceConnection) {

	deviceConnection = &_deviceConnection;

	for (int i = 0; i < deviceLength; i++) {
		devices[i]->init();
		if(devices[i]->interruptEnabled){
			#if(ENABLE_DEVICE_INTERRUPTION)
				PIN_INTERRUPT = &arduinoInterruptedPin;
				enableInterrupt(devices[i]->pin, &(OpenDeviceClass::onInterruptReceived), devices[i]->interruptMode);
			#endif
		}
	}


	if(deviceConnection){
		deviceConnection->setDefaultListener(&(OpenDeviceClass::onMessageReceived));
	}

	deviceConnection->begin();

	_afterBegin();

	// ODev.debug("Begin [OK]");

}


void OpenDeviceClass::_loop() {

	if(deviceConnection){
		deviceConnection->checkDataAvalible();
	}

	checkSensorsStatus();

	// Send PING/KeepAlive if enabled
	if(connected && keepAliveEnabled){
	  unsigned long currentMillis = millis();
	  if(currentMillis - keepAliveTime > KEEP_ALIVE_INTERVAL) {
		keepAliveTime = currentMillis;
		keepAliveMiss++;

		ODev.send(cmd(CommandType::PING_REQUEST));
		if(keepAliveMiss > KEEP_ALIVE_MAX_MISSING){
			connected = false;
		}
	  }
	}
	

}

void OpenDeviceClass::enableKeepAlive(bool val){
	keepAliveEnabled = val;
}

void OpenDeviceClass::enableDebug(uint8_t _debugTarget){
	Config.debugMode = true;
	Config.debugTarget = _debugTarget;
	if(_debugTarget == DEBUG_SERIAL) Serial.begin(DEFAULT_BAUD);
}

/** Called when a command is received by the connection */
void OpenDeviceClass::onMessageReceived(Command cmd) {
	ODev.lastCMD = cmd;
	DeviceConnection *conn = ODev.deviceConnection;

//	if(!ODev.connected){
//		pinMode(13, OUTPUT);
//		digitalWrite(13, HIGH);
//	}

	ODev.connected = true;
	ODev.keepAliveTime = millis();
	ODev.keepAliveMiss = 0;

	bool cont = true; // TODO: Chama handlers(functions), se retornar false abota a continuacao;

	ODev.debug("CType:", cmd.type);
	// ODev.showFreeRam();

	// Directed to a device (Like On/OFF or more complex)
	if (cmd.deviceID > 0) {
		Device *foundDevice = ODev.getDevice(cmd.deviceID);
		if (foundDevice != NULL) {
			ODev.debugChange(foundDevice->id, cmd.value);
			foundDevice->setValue(cmd.value, false);
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
				ODev.notifyReceived(ResponseStatus::SUCCESS);
				(*ODev.commands[i].function)();
			}
		}
	} else if (cmd.type == CommandType::PING_REQUEST) {

		ODev.send(ODev.resp(CommandType::PING_RESPONSE, 0, ResponseStatus::SUCCESS));

	} else if (cmd.type == CommandType::RESET) {

		pinMode(Config.pinReset, OUTPUT);
		digitalWrite(Config.pinReset, LOW);

		// Send response Ex: GET_DEVICES_RESPONSE;ID;Length;[ID, PIN, VALUE, TARGET, SENSOR?, TYPE];[ID,PIN,VALUE,...];[ID,PIN,VALUE,...]
	} else if (cmd.type == CommandType::GET_DEVICES) {

		conn->doStart();
		conn->print(CommandType::GET_DEVICES_RESPONSE);
		conn->doToken();
		conn->print(cmd.id);
		conn->doToken();
		conn->print(ODev.deviceLength);
		conn->doToken();

		char buffer[50] = {0};// FIXME: daria para usar o mesmo buffer do deviceConnection ??

		for (int i = 0; i < ODev.deviceLength; ++i) {
			Device *device = ODev.getDeviceAt(i);
			// Write array to connection..
			device->toString(buffer);
			conn->print(buffer);

			memset(buffer, 0, sizeof(buffer));

			if (i < ODev.deviceLength) {
				conn->doToken();
			}
		}

		conn->doEnd();

	}else{

		// TODO: Send response: UNKNOW_COMMAND

	}

}

/**
 * Fired by Device when user change device state in Sketch
 */
bool OpenDeviceClass::onDeviceChanged(uint8_t iid, unsigned long value) {
	ODev.debugChange(iid, value);
	ODev.sendValue(ODev.getDevice(iid)); // sync with server
	return true;
}


void OpenDeviceClass::onInterruptReceived(){

//#ifdef EnableInterrupt_h
    for (int i = 0; i < ODev.deviceLength; i++) {
    	Device *device = ODev.getDeviceAt(i);

    	uint8_t pinChange = *(uint8_t *) PIN_INTERRUPT;

    	if(device->sensor && device->pin == pinChange){
    		if(device->hasChanged()){
    			device->needSync = true;
    		}
    	}
    }
//#endif

}


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

	if(ODev.connected){
		ODev.deviceConnection->send(lastCMD, false);
		// Check extra data to send.
		sensor->serializeExtraData(ODev.deviceConnection);
		ODev.deviceConnection->doEnd();	
	}

}


void OpenDeviceClass::send(Command cmd){
	if(ODev.connected) deviceConnection->send(cmd, true);
}

Command OpenDeviceClass::cmd(CommandType::CommandType type, uint8_t deviceID, unsigned long value){
	Command cmd;
	cmd.type = type;
	cmd.deviceID = deviceID;
	cmd.value = value;
	return cmd;
}

Command OpenDeviceClass::resp(CommandType::CommandType type, uint8_t deviceID, unsigned long value){
	lastCMD.type = type;
	lastCMD.deviceID = deviceID;
	lastCMD.value = value;
	return lastCMD;
}

void OpenDeviceClass::clear(Command cmd){
	lastCMD.type = 0;
	lastCMD.id = 0;
	lastCMD.deviceID = 0;
	lastCMD.value = 0;
	lastCMD.length = 0;
}


/** Send reply stating that the command was received successfully */
void OpenDeviceClass::notifyReceived(ResponseStatus::ResponseStatus status){
  lastCMD.type = CommandType::DEVICE_COMMAND_RESPONSE;
  lastCMD.value = status;
  // FIXME: lastCMD.data =  NULL;
  lastCMD.length = 0;
  send(lastCMD);
}

void OpenDeviceClass::debugChange(uint8_t id, unsigned long value){

	if(Config.debugMode){

		if(Config.debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:CHANGE:");
			deviceConnection->print(id);
			deviceConnection->print("=");
			deviceConnection->print(value);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:CHANGE:");
			Serial.print(id, DEC);
			Serial.print("=");
			Serial.println(value, DEC);
			#endif
		}
	}
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Device* OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type){
	return addSensor(pin, type, 0);
}

Device* OpenDeviceClass::addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID){
	Device* v = addDevice(pin, type, true, 0);
	if(v) devices[deviceLength-1]->targetID = targetID;
	return v;
}


Device* OpenDeviceClass::addSensor(Device& sensor){
	return addDevice(sensor);
}


Device* OpenDeviceClass::addDevice(uint8_t pin, Device::DeviceType type){
	return addDevice(pin, type, false, 0);
}


Device* OpenDeviceClass::addDevice(Device& device){
	if (deviceLength < MAX_DEVICE) {

		if(device.pin > 0){
			if (device.sensor) {
				// default is INPUT.
			} else {
				pinMode(device.pin, OUTPUT);
			}
		}

		if (device.id <= 0) device.id =(deviceLength + 1);

		devices[deviceLength] = &device;
		deviceLength = deviceLength + 1;
		device.setSyncListener(&(OpenDeviceClass::onDeviceChanged));

		return &device;
	} else{
		return false;
	}

}


Device* OpenDeviceClass::addDevice(uint8_t pin, Device::DeviceType type, bool sensor, uint8_t id){
	if (deviceLength < MAX_DEVICE) {

		if (sensor) {
			if (type == Device::DIGITAL) {
				#if defined(INPUT_PULLUP)
				  pinMode(pin, INPUT_PULLUP);
				#else // Aternative INPUT_PULLUP - TODO: not tested !
				  pinMode(pin, INPUT);
				  digitalWrite (pin, HIGH);
				#endif
			}
		} else {
			pinMode(pin, OUTPUT);
		}

		if (id == 0) id = (deviceLength + 1);

		devices[deviceLength] = new Device(id, pin, type, sensor);
		deviceLength++;

		return devices[deviceLength-1];
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

#ifdef _TASKSCHEDULER_H_

	void OpenDeviceClass::addTask(Task& aTask, void (*aCallback)()){
		scheduler.addTask(aTask);
		aTask.setCallback(aCallback);
	}


	void OpenDeviceClass::deleteTask(Task& aTask){
		scheduler.deleteTask(aTask);
	}

#endif


void OpenDeviceClass::checkSensorsStatus(){

	// Arduino DOC (http://arduino.cc/en/Reference/analogRead):
	// Takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times

	if(time == 0) time = millis();

	// don't sample analog/digital more than {READING_INTERVAL} ms
	bool pollingReady = millis() - time > READING_INTERVAL;

	for (int i = 0; i < deviceLength; i++) {

		if(! devices[i]->sensor ) continue;

		bool syncCurrent = false;

		// polling mode
		if(pollingReady && devices[i]->interruptEnabled == false && devices[i]->hasChanged() ){
			syncCurrent = true;
		// interrupt mode
		}else if(devices[i]->interruptEnabled == true && devices[i]->needSync  ){
			syncCurrent = true;
		}

		if(syncCurrent){
			if(devices[i]->notifyListeners()){
				onSensorChanged(devices[i]->id, devices[i]->currentValue);
			}
			devices[i]->needSync = false;
		}

	}

	if(pollingReady) time = millis(); // reset



}

void OpenDeviceClass::setValue(uint8_t id, unsigned long value){
	
	Command lastCMD = ODev.lastCMD;

    for (int i = 0; i < deviceLength; i++) {
    	if(devices[i]->id == id){
    		devices[i]->setValue(value, false);
    		sendValue(devices[i]);
    		break;
    	}
    }
}

void OpenDeviceClass::sendValue(Device* device){
	lastCMD.id = 0;
	lastCMD.type = (uint8_t) device->type;
	lastCMD.deviceID = device->id;
	lastCMD.value = device->currentValue;
	deviceConnection->send(lastCMD, true);
}


void OpenDeviceClass::toggle(uint8_t id){
	Device* device = ODev.getDevice(id);
	setValue(device->id, !device->getValue());
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

	if(index <= deviceLength){
		return devices[index];
	}

    return NULL;
}

uint8_t * OpenDeviceClass::generateID(uint8_t apin){

	if (!Config.saved) { // not loaded
		loadConfig();
		if (!Config.saved || Config.id[0] == 0) { // not saved

			// The first three octets are the "Organizationally Unique Identifier" or OUI, these are assigned to companies.
			// I've use the OUI from GHEO Sa, which is the correct one for Arduino Ethernet.
			// Then the rest of the MAC address or the last three octets is randomly generated.
			Config.id[0] = 0x90; Config.id[1] = 0xA2; Config.id[2] = 0xDA;

			randomSeed(analogRead(apin));
			//Serial.print("MAC.NOTSAVED -");
			for (int i = 3; i < 6; i++) {
				Config.id[i] = random(0, 255);
			}
			saveConfig();
		}
	}

	//Serial.print("MAC.SAVED:");
	for (int i = 0; i < 6; ++i) {
		Serial.print(Config.id[i], DEC);
		Serial.print(".");
	}
	Serial.println();
	delay(200);

	return Config.id;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END: DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenDeviceClass::showFreeRam() {

	#if(ARDUINO && !defined(ESP8266))
	  extern int __heap_start, *__brkval;

	  int v;


	  #if defined (E2END)
	  //Serial.print(F("DB:EPROM:"));
	  Serial.print(E2END);
	  Serial.print("-");
	  #endif


	  //Serial.print(F("DB:RAM:"));
	  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval), DEC);
  #endif

  #if defined(ESP8266)
	  Serial.print(F("DB:RAM:"));
	  Serial.println(ESP.getFreeHeap());
	  Serial.print(F("DB:EPROM:"));
	  Serial.println(ESP.getFlashChipSize());
  #endif

}


void OpenDeviceClass::debug(const char str[], unsigned long value){
	if(Config.debugMode){ // FIXME: a logica não está muito legal não... !
		if(Config.debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:");
			deviceConnection->print(str);
			if(value >= 0) deviceConnection->print(value);	
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:");
			Serial.print(str);
			if(value >= 0) Serial.print(value);
			if(deviceConnection->conn != &Serial) Serial.println();
			Serial.write(Command::ACK_BIT);
			#endif
		}
	}
}


#ifdef ARDUINO
void OpenDeviceClass::debug(const String &str){
	if(Config.debugMode){ // FIXME: a logica não está muito legal não... !
		if(Config.debugTarget == 1){
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


