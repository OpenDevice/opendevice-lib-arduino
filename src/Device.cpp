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

/*
 * Device.cpp
 *
 *  Created on: 05/02/2013
 *  Author: Ricardo JL Rufino
 */

#include "Device.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Device
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Device::Device(){
	_init(NULL, 0, 0, DIGITAL, false);
}

Device::Device(uint16_t ipin){
	_init(NULL, 0, ipin, DIGITAL, false);
}

Device::Device(uint16_t ipin, DeviceType type){
	_init(NULL, 0, ipin, type, false);
}

Device::Device(uint16_t ipin, DeviceType type, bool sensor){
	_init(NULL, 0, ipin, type, sensor);
}

Device::Device(uint8_t _id, uint16_t _pin, DeviceType _type){
	_init(NULL, _id, _pin, _type, false);
}

Device::Device(uint8_t _id, uint16_t _pin, DeviceType _type, bool _sensor){
	_init(NULL, _id, _pin, _type, _sensor);
}


void Device::_init(char* name, uint8_t _id, uint16_t _pin, DeviceType _type, bool _sensor){

	id = _id;
	pin = _pin;
	type = _type;
	sensor = _sensor;
	targetID = 0;
	needSync = false;
	inverted = false;
	interruptEnabled = false;
	interruptMode = CHANGE;
	changeListener = 0;
	deviceName = name;
	readInterval = 0;
	readLastTime = 0;
	currentValue = 0;
	ioExtender = NULL;
	filter = NULL;
}


void Device::on(){
	setValue(HIGH, true);
}

void Device::off(){
	setValue(LOW, true);
}

void Device::toggle(){
	setValue(!currentValue, true);
}

bool Device::setValue(value_t value, bool sync){
	currentValue = value;

	if(sensor == false){
		if(type == Device::DIGITAL){

			if(inverted) value = ! value;

			_digitalWrite(pin, (value == 0 ? LOW : HIGH));
		}else{
			_analogWrite(pin, value);
		}

		notifyListeners(); // Notify internal listeners (onChange)

		// Send value to server/client
		if(sync){
			if(syncListerner) (*syncListerner)(id, currentValue);
		}
	}

	return true;
}

value_t Device::getValue(){

	if(sensor){
		return currentValue; // return last filtered value... (see: hasChanged)
	}else{
		if(type == Device::DIGITAL){

			uint8_t value = _digitalRead(pin);

			if(inverted) value = ! value;

			return value;
		}else if(type == Device::BOARD){
			return 0;
		}else{
			return _analogRead(pin);
		}
	}

}

size_t Device::serializeExtraData(DeviceConnection *conn){
	return -1;
}

void Device::deserializeExtraData(Command *cmd, DeviceConnection *conn){
	// do nothing
}


bool Device::canReadSensor(){
	if(readInterval > 0){ // Check elapsed time

		// First read
		if(readLastTime == 0){
			readLastTime = millis();
			return true;
		}

		if(millis() - readLastTime >= readInterval){
			readLastTime = millis();
			return true;
		}

		return false;

	}else{
		return true;
	}
}

// FOR SENSOR device...
bool Device::hasChanged(){

	value_t v = 0;

	if(type == Device::DIGITAL){
		v = _digitalRead(pin); // READ and Invert state because (sensor) is a INPUT_PULLUP
		if(inverted) v = !v;
	}else{
		v = _analogRead(pin);
		//if(currentValue != v){
			// call analog filter...
		//}
	}

	// check if has changed
	if(currentValue != v){
		currentValue = v;
		return true;
	}

	return false;
}


int Device::_digitalRead(uint16_t pin){
	if(ioExtender) return ioExtender->digitalReadEx(pin);
	else return digitalRead(pin);
}

int Device::_analogRead(uint16_t pin){
	if(ioExtender) return ioExtender->analogReadEx(pin);
	else return analogRead(pin);
}

void Device::_analogWrite(uint16_t pin, int val){
	if(ioExtender) ioExtender->analogWriteEx(pin, val);
	else analogWrite(pin, val);
}

void Device::_digitalWrite(uint16_t pin, uint8_t val){
	if(ioExtender) ioExtender->digitalWriteEx(pin, val);
	else digitalWrite(pin, val);
}

/**
 * This will be called on ODev.begin() to do extra initialization
 */
void Device::init(){

	// Ignore default init() for io expander.
	if(ioExtender){
		// ioExtender.init();
		return;
	}

	// pinMode INIT
	if(pin > 0 && type != Device::BOARD){

		// sensor - opering in inverted mode
		if(inverted && type == Device::DIGITAL){

			if(sensor){
				#if defined(INPUT_PULLUP)
				  pinMode (pin, INPUT_PULLUP);
				#else //TODO: not tested !
				  pinMode (pin, INPUT);
				  digitalWrite (pin, HIGH);
				#endif
			}else{
				pinMode(pin, OUTPUT);
				digitalWrite(pin, (currentValue == LOW ? HIGH : LOW));
			}

		}else{
			if (sensor) {
				pinMode(pin, INPUT);
			} else {
				pinMode(pin, OUTPUT);
			}
		}

	}

}

Device* Device::enableInterrupt(uint8_t mode){
	if(sensor){
		interruptEnabled = true;
		interruptMode = mode;
	}
	return this;
}

Device* Device::invertedState(){
	inverted = true;
	return this;
}

Device* Device::setInterval(int32_t _interval){
	readInterval = _interval;
	return this;
}

void Device::name(const char* name){
	deviceName = name;
}

const char* Device::name(){
	return deviceName;
}

void Device::onChange(DeviceListener listener){
	changeListener = listener;
}

void Device::setSyncListener(DeviceListener listener){
	syncListerner = listener;
}

Device* Device::setFilter(ValueFilter* _filter){
	filter = _filter;
	filter->setDevice(this);
	return this;
}

Device* Device::setIOExtender(IOExtender* _extender){
	ioExtender = _extender;
	return this;
}

/**
 * fire 'onChange' listener
 */
bool Device::notifyListeners(){
	if(changeListener){
		return (*changeListener)(id, currentValue);
	} else{
		return true; // force sync with server
	}
}


// [NAME, ID, PIN, VALUE, TARGET, SENSOR?, TYPE]
int Device::toString(Print* conn){

	char v_str[6];
	/* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
	dtostrf(getValue(), 4, 2, v_str);

	conn->print('[');

	#if(ENABLE_PREFIX_NAME)
		if(type != Device::BOARD){
			conn->print(od::Config.moduleName);
			conn->print("::");
		}
	#endif

	conn->print(deviceName);
	conn->print(',');
	conn->print(id);
	conn->print(',');
	conn->print(pin);
	conn->print(',');
	conn->print(v_str);
	conn->print(',');
	conn->print(targetID);
	conn->print(',');
	conn->print((sensor ? 1 : 0));
	conn->print(',');
	conn->print((int)type);
	conn->print(']');

//	int size = sprintf (buffer, "[%s,%d,%d,%s,%d,%d,%d]",
//				(name != NULL ? name : ""),
//				id, pin, v_str, targetID, (sensor ? 1 : 0), itype);

//  Serial.print(">>>>>> Device :");
//  Serial.println(buffer);
	 return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Value Filter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


ValueFilter::ValueFilter() : device(NULL) {

}

ValueFilter::~ValueFilter() {

}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Value Filter
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


IOExtender::IOExtender() {

}

IOExtender::~IOExtender() {

}




