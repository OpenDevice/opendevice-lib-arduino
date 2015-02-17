/*
 * DeviceManager.cpp
 *
 *  Created on: 05/02/2013
 *      Author: Ricardo JL Rufino
 */

#include "DeviceManager.h"

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

	if(_sensor){
		currentValue = LOW;
	}else{
		currentValue = HIGH; // TODO: isso deve ser definido, pois tomada é HIGHT e lanpada deve ser LOW
	}
}

bool Device::setValue(unsigned long value){
	currentValue = value;

	if(!sensor){
		if(type == Device::DIGITAL){
			digitalWrite(pin, (value == 1 ? HIGH : LOW));
		}else{
			analogWrite(pin, value);
		}
	}

	return true;
}

unsigned long Device::getValue(){

	if(sensor){
		return currentValue; // return last filtered value... (see: hasChanged)
	}else{
		if(type == Device::DIGITAL){
			return digitalRead(pin);
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
		v = ! digitalRead(pin); // READ and Invert state because is a INPUT_PULLUP
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


// [ID, PIN, VALUE, TARGET, SENSOR?, TYPE]
int Device::toString(char buffer[]){
	return sprintf (buffer, "[%d,%d,%d,%d,%d,%d]", id, pin, getValue(), targetID, sensor, type);
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DeviceManager
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


DeviceManager::DeviceManager() {
	_init();
}

void DeviceManager::setDefaultListener(void (*pt2Func)(uint8_t, unsigned long)){
	callbackPtr = pt2Func;
}

void DeviceManager::_init(){
	time = 0;
	deviceLength = 0;
	for (int i = 0; i < MAX_DEVICE; i++) {
		devices[i] = NULL;
	}
}


void DeviceManager::init(){
    for (int i = 0; i < deviceLength; i++) {
    	devices[i]->init();
    }
}


bool DeviceManager::addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID){
	bool v = addDevice(pin, type, true, 0);
	if(v) devices[deviceLength-1]->targetID = targetID;
	return v;
}

bool DeviceManager::addDevice(uint8_t pin, Device::DeviceType type){
	return addDevice(pin, type, false, 0);
}


bool DeviceManager::addDevice(Device& device){
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


bool DeviceManager::addDevice(uint8_t pin, Device::DeviceType type, bool sensor, uint8_t id){
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

void DeviceManager::checkStatus(){

	// Arduino DOC (http://arduino.cc/en/Reference/analogRead):
	// Takes about 100 microseconds (0.0001 s) to read an analog input, so the maximum reading rate is about 10,000 times

	if(time == 0) time = millis();

	if (millis() - time > READING_INTERVAL){ // don't sample analog/digital more than XXXms
	    for (int i = 0; i < deviceLength; i++) {

	    	if(devices[i]->sensor && devices[i]->hasChanged()){
	    		(*callbackPtr)(devices[i]->id, devices[i]->currentValue); // call calback function..
	    	}

	    }

	    time = millis();
	}

}

void DeviceManager::setValue(uint8_t id, unsigned long value){
    for (int i = 0; i < deviceLength; i++) {
    	if(devices[i]->id == id){
    		devices[i]->setValue(value);
    		break;
    	}
    }
}

void DeviceManager::sendToAll(unsigned long value){
    for (int i = 0; i < deviceLength; i++) {
    	devices[i]->setValue(value);
    }
}

Device* DeviceManager::getDevice(uint8_t id){

//	Serial.print("DB: getDevice :: size: ");
//	Serial.print(deviceLength, DEC);
//	Serial.print(" > ");

    for (int i = 0; i < deviceLength; i++) {

//    	Serial.print(devices[i]->id);
//    	Serial.print(",");

    	if(devices[i]->id == id){
//    		Serial.println("DB: getDevice TRUE");
//    		Serial.write(19);
    		return devices[i];
    	}
    }

//	Serial.println("DB: getDevice FALSE");
//	Serial.write(19);

    return NULL;
}

Device* DeviceManager::getDeviceAt(uint8_t index){

	if(index > 0 && index <= deviceLength){
		return devices[index-1];
	}

    return NULL;
}

