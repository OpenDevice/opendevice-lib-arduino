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

#if defined(_RCSwitch_h)

#include "RFSensor.h"

RFSensor::RFSensor(byte interruptPinPort) :
		Device(0, 0, Device::NUMERIC, true), rf() {

	rf.enableReceive(interruptPinPort);

}

RFSensor::~RFSensor() {

}

bool RFSensor::hasChanged(){

	if (rf.available()) {

		int value = rf.getReceivedValue();

		if (value == 0) {
			return false;
		}

		currentValue = (unsigned long) value;

		rf.resetAvailable();

		return true;
	}


	return false;

}

#endif
