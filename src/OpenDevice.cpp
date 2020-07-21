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
OpenDeviceClass::OpenDeviceClass() :
		saveAndDebugTimer(SAVE_DEVICE_INTERVAL, true),
		resetTimer(RESET_TIMEOUT, false){
	deviceConnection = NULL;
	autoControl = false;
	keepAliveTime = 0;
	keepAliveMiss = 0;
	time = 0;
	deviceLength = 0;
	commandsLength = 0;
	needSaveDevices = false;
	syncDone = false;

	if(SAVE_DEVICE_INTERVAL == 0) saveAndDebugTimer.disable();

	for (int i = 0; i < MAX_DEVICE; i++) {
		devices[i] = NULL;
	}

	#if defined(ESP8266)
		EEPROM.begin(sizeof(Config));
	#endif

	load(); // Load storage configuration
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

	// Setup debug target for Logger..
	if(!Logger.out){
		if(Config.debugTarget == DebugTarget::DEBUG_SERIAL){
			Logger.out = &Serial;
		}else{
			Logger.out = deviceConnection;
		}
	}

	LOG_DEBUG("ModuleName", Config.moduleName);
	LOG_DEBUG("Server", Config.server);

	char version[30];
	strcpy(version, API_VERSION);
	strcat(version, "@");
	strcat(version, FirmwareBuildDate);
	
	LOG_DEBUG("Firmware", version); // from build_defs.h

	for (int i = 0; i < deviceLength; i++) {
		devices[i]->init();
		if(devices[i]->interruptEnabled){
			#if(ENABLE_DEVICE_INTERRUPTION)
				PIN_INTERRUPT = &arduinoInterruptedPin;
				enableInterrupt(devices[i]->pin, &(OpenDeviceClass::onInterruptReceived), devices[i]->interruptMode);
			#endif
		}
	}

	// Load Device(ID) / Value from Storage and set in devices
	// restoreDevicesFromStorage();

	// Trace restarts using VALUE of board
	devices[0]->currentValue++;

	if(deviceConnection){
		deviceConnection->setDefaultListener(&(OpenDeviceClass::onMessageReceived));
	}

	deviceConnection->begin();

	_afterBegin();

}


void OpenDeviceClass::_loop() {

	loops++;

	if(deviceConnection){

		deviceConnection->checkDataAvalible();

		// Send PING/KeepAlive if enabled
		if(Config.keepAlive){
		  unsigned long currentMillis = millis();
		  if(currentMillis - keepAliveTime > KEEP_ALIVE_INTERVAL) {
				keepAliveTime = currentMillis;
				keepAliveMiss++;
				send(cmd(CommandType::PING_REQUEST));
				if(keepAliveMiss > KEEP_ALIVE_MAX_MISSING){
					deviceConnection->connected = false;
				}
		  }
		}
	}

	checkSensorsStatus();

	if(saveAndDebugTimer.expired()){

		// Debug info
		#if DEBUG && defined(SHOW_DEBUG_STATE)

		if(deviceConnection->conn != &Serial){ // don't show if using serial communication

			Logger.printLoop('=', 23, false);
			Serial.print(" [ OpenDevice ]");
			Logger.printLoop('=', 23);

			Serial.print("= ["); Serial.print(Config.moduleName);
			Serial.print("] || Server: "); Serial.print(Config.server);
			Serial.print(" || Devices: "); Serial.print(Config.devicesLength);
			Serial.println();

			Serial.print("= Uptime: "); Serial.print((millis() / 1000) / 60); Serial.print("min");
			Serial.print(" || Loops/ms: "); Serial.print(loops / SAVE_DEVICE_INTERVAL);
			Serial.print(" || Restarts: "); Serial.print(devices[0]->currentValue);
			Serial.println();

			Serial.print("= Connected: "); Serial.print(isConnected());

			#if defined(ESP_h)
				wl_status_t status = WiFi.status();
				Serial.print(" || WIFI: "); Serial.print( (status == WL_CONNECTED ) );
				if(status == WL_CONNECTED){
					Serial.print("("); Serial.print(WiFi.RSSI());
					Serial.print(") || IP: "); Serial.print(WiFi.localIP());
				} 
				Serial.println();
				Serial.print("= RAM: "); Serial.print(ESP.getFreeHeap());
				Serial.print(" || EPROM/SIZE: "); Serial.print(ESP.getFlashChipSize());
			#endif
				Serial.println();

			loops=0;

			Logger.printLoop('=', 61);

		}

		#endif

		// Save
		if(needSaveDevices){
			unsigned long time = micros();

			for (int i = 0; i < deviceLength; ++i) {
				Device *device = getDeviceAt(i);
				Config.devicesState[i] = device->currentValue;
			}

			save(); // TODO: improve saving only device state
			saveAndDebugTimer.reset();
			needSaveDevices = false;

			Logger.debug("Saving devices, time(uS)", micros() - time);
		}


	}


	// Check reset
	if(Config.pinReset != 255 && digitalRead(Config.pinReset) == LOW){

		if(!resetTimer.isEnabled()){
			LOG_DEBUG_S("Starting timmer...");
			resetTimer.enable(); // neable timer
		}

		if(resetTimer.expired()){
			reset();
		}
	}else if(Config.pinReset != 255 && resetTimer.isEnabled()){ // PIN released before reset
		resetTimer.disable();
	}
}

void OpenDeviceClass::enableKeepAlive(bool val){
	Config.keepAlive = val;
}


void OpenDeviceClass::enableDebug(uint8_t _debugTarget){
	Config.debugMode = true;
	Config.debugTarget = _debugTarget;
	if(_debugTarget == DEBUG_SERIAL) Serial.begin(DEFAULT_BAUD);
}

void OpenDeviceClass::enableDebug(Print &out){
	Config.debugMode = true;
	Config.debugTarget = DebugTarget::DEBUG_CUSTOM;
	Logger.out = &out;
}

/**
 * Fired by Device when user change device state in Sketch
 */
bool OpenDeviceClass::onDeviceChanged(uint8_t iid, value_t value) {
	ODev.needSaveDevices = true;
	// Send only when sync done..
	if(ODev.syncDone){
		Device* device = ODev.getDevice(iid);
		ODev.debugChange(device);
		ODev.sendValue(device); // sync with server
	}
	return true;
}

void OpenDeviceClass::onMessageReceived(Command cmd) {
	ODev.messageReceived = true;
	ODev.lastCMD = cmd;
}

void OpenDeviceClass::onInterruptReceived(){

//#ifdef EnableInterrupt_h
    for (int i = 0; i < ODev.deviceLength; i++) {
    	Device *device = ODev.getDeviceAt(i);

    	uint8_t pinChange = *(uint8_t *) PIN_INTERRUPT;

    	if(device->sensor && device->pin == pinChange){
    		if(device->canReadSensor() && device->hasChanged()){
    			device->needSync = true;
    		}
    	}
    }
//#endif

}


void OpenDeviceClass::onSensorChanged(Device* sensor){
	needSaveDevices = true;
	Device* device = getDevice(sensor->targetID);

//    if(id < 100) // FIXME: detectar se é o IR.
//    	value = !value;        // NOTA: Os valores do Swicth sao invertidos

	debugChange(sensor);

	if(autoControl){
		if(device != NULL){
			// Sepre que uma alteracao for detectada, será invertido o seu estado atual.
			// Caso o dispositivo seja digital, ele ira reconhecer apenas dois valores, 0..1
			if(device->type == Device::DIGITAL){
				device->setValue( ! device->getValue() );
			}else{
				value_t cval = device->getValue();
				if(cval == 0) device->setValue(Device::MAX_ANALOG_VALUE); // set max
				else device->setValue(0); // set max
			}
		}
	}

	// SEND: Command
	// ==========================
	lastCMD.id = 0;
    lastCMD.type = Device::TypeToCommand(sensor->type);
	lastCMD.deviceID = sensor->id;
	lastCMD.value = sensor->currentValue;

	if(deviceConnection->connected){
		deviceConnection->send(lastCMD, false);
		// Check extra data to send.
		sensor->serializeExtraData(deviceConnection);
		deviceConnection->doEnd();
	}

}


void OpenDeviceClass::send(Command cmd){
	deviceConnection->send(cmd, true);
}

void OpenDeviceClass::sendCustomCommand(const char cmdName[], volatile CustomComandPtr func) {
	
	if(syncDone && deviceConnection->canSend()){
		deviceConnection->doStart();
		deviceConnection->print(CommandType::USER_COMMAND);
		deviceConnection->putSeparator();
		deviceConnection->print(0); // cmdID
		deviceConnection->putSeparator();
		deviceConnection->print(cmdName);
		deviceConnection->putSeparator();
		func(deviceConnection); 
		deviceConnection->doEnd();
	}else{
		LOG_DEBUG("Can't send.");
	}
}

void OpenDeviceClass::sendCustomCommand(const char cmdName[], double length, ...){
	
	if(syncDone && deviceConnection->canSend()){
		deviceConnection->doStart();
		deviceConnection->print(CommandType::USER_COMMAND);
		deviceConnection->putSeparator();
		deviceConnection->print(0); // cmdID
		deviceConnection->putSeparator();
		deviceConnection->print(cmdName);
		deviceConnection->putSeparator();

		va_list args;
    	va_start(args, length);
		double value;
		for (uint8_t i = 0; i < length - 1; ++i){
			value = va_arg(args, double);
			deviceConnection->print(value);
			deviceConnection->putSeparator();
		}
		deviceConnection->print(va_arg(args, double));
		va_end(args);

		deviceConnection->doEnd();
	}else{
		LOG_DEBUG("Can't send.");
	}

}


Command OpenDeviceClass::cmd(CommandType::CommandType type, uint8_t deviceID, value_t value){
	Command cmd;
	cmd.type = type;
	cmd.deviceID = deviceID;
	cmd.value = value;
	cmd.length = 0;
	return cmd;
}

Command OpenDeviceClass::resp(CommandType::CommandType type, uint8_t deviceID, value_t value){
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

void OpenDeviceClass::debugChange(Device* device){

	#if DEBUG
	if(Config.debugMode){

		if(Config.debugTarget == 1){
			deviceConnection->doStart();
			deviceConnection->print("DB:CHANGE ");
			deviceConnection->print(device->deviceName);
			deviceConnection->print("=");
			deviceConnection->print(device->currentValue);
			deviceConnection->doEnd();
		}else{
			#if(ENABLE_SERIAL)
			Serial.print("DB:CHANGE:");
			Serial.print(device->deviceName);
			Serial.print("=");
			Serial.println(device->currentValue, DEC);
			#endif
		}
	}
	#endif
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Device* OpenDeviceClass::addSensor(const char* name, uint16_t pin, Device::DeviceType type){
	return addSensor(name, pin, type, 0);
}

Device* OpenDeviceClass::addSensor(const char* name, uint16_t pin, Device::DeviceType type, uint8_t targetID){
	Device* v = addDevice(name, pin, type, true, 0);
	if(v) devices[deviceLength-1]->targetID = targetID;
	return v;
}

Device* OpenDeviceClass::addSensor(const char* name, Device& sensor){
	return addDevice(name, sensor);
}

Device* OpenDeviceClass::addDevice(const char* name, uint16_t pin, Device::DeviceType type){
	return addDevice(name, pin, type, false, 0);
}

Device* OpenDeviceClass::addDevice(Device& device){
	return addDevice(NULL, device);
}


Device* OpenDeviceClass::addDevice(const char* name, Device& device){
	if (deviceLength < MAX_DEVICE) {

		// Force syncronization with server
		#if(ENABLE_SYNC_DEVICEID)
			if (device.id <= 0) device.id = 0;
		#else
			if (device.id <= 0) device.id = (deviceLength + 1); // auto increment
		#endif

		devices[deviceLength] = &device;
		deviceLength = deviceLength + 1;
		device.setSyncListener(&(OpenDeviceClass::onDeviceChanged));

		const char* deviceName = name;

		#if(ENABLE_PREFIX_NAME)
			// if(device.type != Device::BOARD){
			// 	deviceName = (String(Config.moduleName)+String(name)).c_str();
			// 	Logger.debug("Concatenation", deviceName);
			// }
		#endif

		// LOG_DEBUG("Add Device", deviceName);
		device.name(deviceName);

		return &device;
	} else{
		return NULL;
	}
}

Device* OpenDeviceClass::addDevice(const char* name, uint16_t pin, Device::DeviceType type, bool sensor, uint8_t id){
	devices[deviceLength] = new Device(id, pin, type, sensor);
	return addDevice(name, *devices[deviceLength]);
}

// Device* OpenDeviceClass::addDevice(char* name, uint8_t pin, Device::DeviceType type, bool sensor, uint8_t id){
// 	if (deviceLength < MAX_DEVICE) {

// 		// Force syncronization with server
// 		#if(ENABLE_SYNC_DEVICEID)
// 		// nothing.. id == 0
// 		#else
// 		if (id == 0) id = (deviceLength + 1);
// 		#endif

// 		devices[deviceLength] = new Device(id, pin, type, sensor);
// 		devices[deviceLength]->name(name);
// 		devices[deviceLength]->setSyncListener(&(OpenDeviceClass::onDeviceChanged));
// 		deviceLength++;

// 		return devices[deviceLength-1];
// 	} else{
// 		return false;
// 	}
// }

/**
 * Register a custom command to trigger a user function.
 * Parameters can be used, this cab be get using (ODev.readString(), ODev.readInt())
 * For details se 'examples/EthernetCustomCommands'
 */
bool OpenDeviceClass::addCommand(const char * name, void (*function)()){
	if (commandsLength < MAX_COMMAND) {

//			strncpy(commands[commandsLength].command, name , strlen(name));
			commands[commandsLength].command = name;
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

	if(time == 0) time = micros();

	// don't sample analog/digital more than {READING_INTERVAL} ms
	bool pollingReady = micros() - time > READING_INTERVAL;

	for (int i = 0; i < deviceLength; i++) {

		if(! devices[i]->sensor ) continue;

		bool syncCurrent = false;

		bool canReadSensor = (pollingReady && devices[i]->canReadSensor()); // check elapsed interval (if exist)

		// polling mode
		if(canReadSensor && devices[i]->interruptEnabled == false && devices[i]->hasChanged()){
			syncCurrent = true;
		// interrupt mode
		}else if(devices[i]->interruptEnabled == true && devices[i]->needSync  ){
			syncCurrent = true;
		}

		if(syncCurrent){
			if(devices[i]->notifyListeners()){
				onSensorChanged(devices[i]);
			}
			devices[i]->needSync = false;
		}

	}

	if(pollingReady) time = micros(); // reset



}

void OpenDeviceClass::setValue(uint8_t id, value_t value){

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
	lastCMD.type = (uint8_t) Device::TypeToCommand(device->type);
	lastCMD.deviceID = device->id;
	lastCMD.value = device->currentValue;
	deviceConnection->send(lastCMD, true);
}


void OpenDeviceClass::toggle(uint8_t index){
	Device* device = getDeviceAt(index);
	setValue(device->id, !device->getValue());
}

void OpenDeviceClass::sendToAll(value_t value){
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

Device* OpenDeviceClass::getDevice(const char* name){


    for (int i = 0; i < deviceLength; i++) {

    	if(strcmp(devices[i]->deviceName,name) == 0){
    		return devices[i];
    	}
    }


    return NULL;
}

uint8_t * OpenDeviceClass::generateID(uint8_t apin){

	if (Config.id[0] == 0 && Config.id[1] == 0) { // not saved

		// The first three octets are the "Organizationally Unique Identifier" or OUI, these are assigned to companies.
		// I've use the OUI from GHEO Sa, which is the correct one for Arduino Ethernet.
		// Then the rest of the MAC address or the last three octets is randomly generated.
		Config.id[0] = 0x90; Config.id[1] = 0xA2; Config.id[2] = 0xDA;

		randomSeed(analogRead(apin));
		//Serial.print("MAC.NOTSAVED -");
		for (int i = 3; i < 6; i++) {
			Config.id[i] = random(0, 255);
		}
		save();
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

void OpenDeviceClass::name(const char *pname){
	strcpy (Config.moduleName, pname);
	addDevice(Config.moduleName, 0, Device::BOARD); // Add Board Device 
}

void OpenDeviceClass::server(char pname[]){
	strcpy(Config.server, pname);
}

void OpenDeviceClass::apiKey(char pname[]){
	strcpy(Config.appID, pname);
}

void OpenDeviceClass::resetPin(byte pin){
	Config.pinReset = pin;
	pinMode(pin, INPUT_PULLUP);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END: DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenDeviceClass::showFreeRam() {

	#if(ARDUINO && !defined(ESP8266))
	  extern int __heap_start, *__brkval;
	  int v;

	  #if defined (E2END)
	  Serial.print(F("DB:E:"));
	  Serial.println(E2END);
	  #endif


	  Serial.print(F("DB:R:"));
	  Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval), DEC);
  #endif

  #if defined(ESP8266)
	  Serial.print(F("DB:RAM:"));
	  Serial.println(ESP.getFreeHeap());
	  Serial.print(F("DB:EPROM:"));
	  Serial.println(ESP.getFlashChipSize());
  #endif

}


void OpenDeviceClass::reset() {
	#if defined(ESP8266)
		LOG_DEBUG(F("DB:Disconnecting..."));
		if(deviceConnection) deviceConnection->disconnect();
		delay(1000);
		LOG_DEBUG(F("DB:Reseting..."));
		ESP.reset();
		delay(2000);
	#else
		pinMode(Config.pinReset, OUTPUT);
		digitalWrite(Config.pinReset, LOW);
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

/**
 * Verify if was saved the devices ID and value on the internal storage.
 * If not saved, then the server will send IDs.
 */
void OpenDeviceClass::restoreDevicesFromStorage(){
	#if(LOAD_DEVICE_STORAGE)

		LOG_DEBUG("Restore Devices", Config.devicesLength);

		// Only restore IDs if has no change
		// Otherise IDs will loaded from server
		if(Config.devicesLength == deviceLength){
			for (int i = 0; i < deviceLength; ++i) {
				Device *device = getDeviceAt(i);
				if(device->id == 0 && i < Config.devicesLength){
					device->id = Config.devices[i];

					// restorevalue, ignore digital sensors
					if(! (device->sensor && device->type == Device::DIGITAL)){
						// Disable listeners on startup....
						DeviceListener listener = device->changeListener;
						device->changeListener = NULL;
						device->setValue(Config.devicesState[i], false);
						device->changeListener = listener;
					}

					//Logger.debug(device->deviceName, Config.devicesState[i]);
//					if(Config.debugMode){
//						Serial.print(device->deviceName);
//						Serial.print(" = ");
//						Serial.println(Config.devicesState[i]);
//					}

				}
			}
		}

	#endif

	Config.devicesLength = deviceLength;
}

OpenDeviceClass ODev;
