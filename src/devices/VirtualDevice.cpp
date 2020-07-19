/*
 * VirtualDevice.cpp
 *
 *  Created on: 27 de ago de 2018
 *      Author: ricardo
 */

#include "VirtualDevice.h"

VirtualDevice::VirtualDevice(DeviceType _type) : Device(0, 0, _type, false) {
	currentValue = 0;
}

VirtualDevice::~VirtualDevice() {
}

value_t VirtualDevice::getValue(){
	return currentValue;
}

bool VirtualDevice::setValue(value_t value, bool sync){
	currentValue = value;

	if(sensor == false){

		notifyListeners(); // Notify internal listeners (onChange)

		// Send value to server/client
		if(sync){
			if(syncListerner) (*syncListerner)(id, currentValue);
		}
	}

	return true;
}

void VirtualDevice::init(){

}

