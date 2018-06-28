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

#include "CustomSensor.h"

CustomSensor::CustomSensor(CommandFunction func) : Device(0, 0, Device::ANALOG, true) , commandFunction(func) {

}

CustomSensor::~CustomSensor() {
}

bool CustomSensor::hasChanged(){

	value_t v = commandFunction();

	// check if has changed
	if(currentValue != v){
		currentValue = v;
		return true;
	}

	return false;

}
