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
#include "config.h"
#include "DeviceConnection.h"

extern "C"
{
  // Definition of the listener function
  typedef bool (*DeviceListener) (uint8_t iid, value_t value);
}

class Device
{
public:

	enum DeviceType{
    DIGITAL = 1,
    ANALOG = 2,
    ANALOG_SIGNED = 3,
    NUMERIC = 4,
    FLOAT2 = 5,  // 2 decimal places
    FLOAT2_SIGNED = 6,
    FLOAT4 = 7,  // 4 decimal places
    CHARACTER = 8,
    //
    BOARD = 10,
    MANAGER = 11
	};
  // NOTE: On change this, my need change: OpenDeviceClass::onSensorChanged

	const static uint8_t MAX_ANALOG_VALUE = 255;

	uint8_t id;
	uint8_t pin;
	value_t currentValue;
	DeviceType type;
	const char* deviceName;

  // For sensor types
	bool sensor;
  int32_t readLastTime;
  int32_t readInterval;

	uint8_t targetID; // associated device (used in sensors)

	bool inverted; // It allows to operate in an inverted logic (activeLow)

	// for interrupt mode
	volatile bool needSync;
	bool interruptEnabled; // only for sensor
	uint8_t interruptMode;

	Device();
	Device(uint8_t ipin);
	Device(uint8_t ipin, DeviceType type);
	Device(uint8_t ipin, DeviceType type, bool sensor);
	Device(uint8_t iid, uint8_t ipin, DeviceType type);
	Device(uint8_t iid, uint8_t ipin, DeviceType type, bool sensor);

	/**
	 * Change value / state of Device
	 * @param sync - sync with server
	 */
	bool setValue(value_t value, bool sync = true);

	void on();

	void off();

	bool isON() { return currentValue == HIGH; }

	bool isOFF() { return currentValue == LOW; }

	/**
	 * Get current value.
	 */
	value_t getValue();

	virtual size_t serializeExtraData(DeviceConnection *conn);

	virtual void deserializeExtraData(Command *cmd, DeviceConnection *conn);

  /** For Sensors ::  read sensor ans check if value has changed */
	virtual bool hasChanged();

  /**  For Sensors ::  If set readInterval has set, check if time as elapsed */
  bool canReadSensor();

	void name(const char* name);

	const char* name();

	/**
	 * Enable to read value using interruptions. <br/>
	 * NOTE: It is necessary to enable support in the general settings.
	 */
	Device* enableInterrupt(uint8_t mode = CHANGE);

	/**
	 * It allows device operate in an inverted logic (the 'ON' command will generate a LOW LEVEL)
	 */
	Device* invertedState();

  Device* activeLow(){ return invertedState(); };

	virtual void init();

  /**
	 * Set reading interval for sensor's
	 */
	Device* setInterval(int32_t _interval);

	void onChange(DeviceListener);

	void setSyncListener(DeviceListener listener);

	bool notifyListeners();

	int toString(char buffer[]);

private:

	DeviceListener changeListener;
	DeviceListener syncListerner;

	void _init(char* name, uint8_t iid, uint8_t ipin, Device::DeviceType type, bool sensor);

};


#endif /* Device_H_ */
