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
	_init(0, 0, DIGITAL, false);
}

Device::Device(uint8_t _id, uint8_t _pin, DeviceType _type){
	_init(_id, _pin, _type, false);
}

Device::Device(uint8_t _id, uint8_t _pin, DeviceType _type, bool _sensor){
	_init(_id, _pin, _type, _sensor);
}

void Device::_init(uint8_t _id, uint8_t _pin, DeviceType _type, bool _sensor){

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

	//if(_sensor){
		currentValue = LOW;
	//}else{
	//	currentValue = LOW; // TODO: isso deve ser definido, pois tomada é HIGHT e lanpada deve ser LOW
	//}

	
}


void Device::on(){
	setValue(HIGH);
}

void  Device::off(){
	setValue(LOW);
}

bool Device::setValue(unsigned long value){
	currentValue = value;

	if(sensor == false){
		if(type == Device::DIGITAL){

			if(inverted) value = ! value;

			digitalWrite(pin, (value == 0 ? LOW : HIGH));
		}else{
			analogWrite(pin, value);
		}

		notifyListeners();
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
		v = ! digitalRead(pin); // READ and Invert state because (sensor) is a INPUT_PULLUP
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
	// Do nothing for now.
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
	digitalWrite(pin, (currentValue == LOW ? HIGH : LOW));
	return this;
}

void Device::onChange(DeviceListener listener){
	changeListener = listener;
}

bool Device::notifyListeners(){
	if(changeListener) return (*changeListener)(currentValue);
	else return true;
}


// [ID, PIN, VALUE, TARGET, SENSOR?, TYPE]
int Device::toString(char buffer[]){
	 int itype = type;
	 return sprintf (buffer, "[%d,%d,%lu,%d,%d,%d]", id, pin, getValue(), targetID, (sensor ? 1 : 0), itype);
}



