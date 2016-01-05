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

#include "FuncSensor.h"

FuncSensor::FuncSensor(CommandFunction func) : Device(0, 0, Device::ANALOG, true) , fgetValue(func) {

}

FuncSensor::~FuncSensor() {
}

bool FuncSensor::hasChanged(){

	unsigned long v = fgetValue();

	// check if has changed
	if(currentValue != v){
		currentValue = v;
		return true;
	}

	return false;

}

