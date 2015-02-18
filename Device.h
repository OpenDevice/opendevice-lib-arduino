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
 * Device.h
 *
 *  Created on: 05/02/2013
 *      Author: Ricardo JL Rufino
 */

#ifndef Device_H_
#define Device_H_

#include <Arduino.h>
#include "Command.h"
#include "DeviceConnection.h"


class Device
{
public:

	enum DeviceType{
			ANALOG = 1,
			DIGITAL = 2,
			// This constans only used in embeeded side, to mapping to command
			INFRA_RED = 7
	};

	const static uint8_t MAX_ANALOG_VALUE = 255;

	uint8_t id;
	uint8_t pin;
	unsigned long currentValue;
	DeviceType type;

	bool sensor;
	uint8_t targetID; // associated device (used in sensors)
	

	Device();
	Device(uint8_t iid, uint8_t ipin, DeviceType type);
	Device(uint8_t iid, uint8_t ipin, DeviceType type, bool sensor);

	/**
	 * Change value / state of Device
	 */
	bool setValue(unsigned long value);

	/**
	 * Get current value.
	 */
	unsigned long getValue();

	virtual size_t serializeExtraData(DeviceConnection *conn);

	virtual void deserializeExtraData(Command *cmd, DeviceConnection *conn);

	virtual bool hasChanged();

	virtual void init();

	void _init(uint8_t iid, uint8_t ipin, Device::DeviceType type, bool sensor);

	int toString(char buffer[]);
};


#endif /* Device_H_ */
