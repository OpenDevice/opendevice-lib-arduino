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

Device::Device(uint8_t ipin){
	_init(NULL, 0, ipin, DIGITAL, false);
}

Device::Device(uint8_t ipin, DeviceType type){
	_init(NULL, 0, ipin, type, false);
}

Device::Device(uint8_t ipin, DeviceType type, bool sensor){
	_init(NULL, 0, ipin, type, sensor);
}

Device::Device(uint8_t _id, uint8_t _pin, DeviceType _type){
	_init(NULL, _id, _pin, _type, false);
}

Device::Device(uint8_t _id, uint8_t _pin, DeviceType _type, bool _sensor){
	_init(NULL, _id, _pin, _type, _sensor);
}


void Device::_init(char* name, uint8_t _id, uint8_t _pin, DeviceType _type, bool _sensor){

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

	//if(_sensor){
		currentValue = LOW;
	//}else{
	//	currentValue = LOW; // TODO: isso deve ser definido, pois tomada é HIGHT e lanpada deve ser LOW
	//}


}


void Device::on(){
	setValue(HIGH, true);
}

void  Device::off(){
	setValue(LOW, true);
}

bool Device::setValue(unsigned long value, bool sync){
	currentValue = value;

	if(sensor == false){
		if(type == Device::DIGITAL){

			if(inverted) value = ! value;

			digitalWrite(pin, (value == 0 ? LOW : HIGH));
		}else{
			analogWrite(pin, value);
		}

		notifyListeners(); // Notify internal listeners (onChange)

		// Send value to server/client
		if(sync){
			if(syncListerner) (*syncListerner)(id, currentValue);
		}
	}

	return true;
}

unsigned long Device::getValue(){

	if(sensor){
		return currentValue; // return last filtered value... (see: hasChanged)
	}else{
		if(type == Device::DIGITAL){

			uint8_t value = digitalRead(pin);

			if(inverted) value = ! value;

			return value;
		}else if(type == Device::BOARD){
			return 0;
		}else{
			return analogRead(pin);
		}
	}

}

size_t Device::serializeExtraData(DeviceConnection *conn){
	return -1;
}

void Device::deserializeExtraData(Command *cmd, DeviceConnection *conn){
	// do nothing
}

// FOR SENSOR device...
bool Device::hasChanged(){

	unsigned long v = 0;

	if(type == Device::DIGITAL){
		v = digitalRead(pin); // READ and Invert state because (sensor) is a INPUT_PULLUP
		if(inverted) v = !v;
	}else{
		v = analogRead(pin);
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

void Device::init(){

	if(inverted && type == Device::DIGITAL){

		if(sensor){
			#if defined(INPUT_PULLUP)
			  pinMode (pin, INPUT_PULLUP);
			#else //TODO: not tested !
			  pinMode (device.pin, INPUT);
			  digitalWrite (device.pin, HIGH);
			#endif
		}else{
			digitalWrite(pin, (currentValue == LOW ? HIGH : LOW));
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

void Device::name(char* name){
	deviceName = name;
}

char* Device::name(){
	return deviceName;
}

void Device::onChange(DeviceListener listener){
	changeListener = listener;
}

void Device::setSyncListener(DeviceListener listener){
	syncListerner = listener;
}

bool Device::notifyListeners(){
	bool ret = true;
	if(changeListener) ret = (*changeListener)(id, currentValue);
	return ret;
}


// [ID, PIN, VALUE, TARGET, SENSOR?, TYPE]
int Device::toString(char buffer[]){
	 int itype = type;
	 return sprintf (buffer, "[%s,%d,%d,%lu,%d,%d,%d]", (deviceName != NULL ? deviceName : ""), id, pin, getValue(), targetID, (sensor ? 1 : 0), itype);
}
