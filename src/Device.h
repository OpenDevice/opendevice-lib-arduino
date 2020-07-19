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

#ifndef Device_H_
#define Device_H_

#include <Arduino.h>
#include "Command.h"
#include "config.h"
#include "DeviceConnection.h"


class Device; // friend declaration

extern "C"
{
  // Definition of the listener function
  typedef bool (*DeviceListener) (uint8_t iid, value_t value);
}

/**
 * Interface for filtering values
 */
class ValueFilter {
public:
	ValueFilter();
	virtual ~ValueFilter();

	virtual bool accept(value_t value){ return false; };

	void setDevice(Device* _device){
		device = _device;
	}

protected:
	Device* device;

};


/**
 * Interface for port IOExtender
 */
class IOExtender {
public:
	IOExtender();
	virtual ~IOExtender();

	virtual int digitalReadEx(uint16_t pin){ return -1;};
	virtual int analogReadEx(uint16_t pin){ return -1;};
	virtual void analogWriteEx(uint16_t pin, int val){};
	virtual void digitalWriteEx(uint16_t pin, uint8_t val){};

	virtual void loop(){};
};

/**
 * Base class of Devices
 * 
 * @date  05/02/2013
 * @author: Ricardo JL Rufino
 */
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


	static uint8_t TypeToCommand( DeviceType type ) {
		switch (type) {
		case Device::DIGITAL: return CommandType::ON_OFF;
		case Device::ANALOG: return CommandType::ANALOG;
		case Device::ANALOG_SIGNED: return CommandType::ANALOG;
		case Device::NUMERIC: return CommandType::NUMERIC;
		case Device::FLOAT2: return CommandType::ANALOG;
		case Device::FLOAT2_SIGNED: return CommandType::ANALOG;
		case Device::FLOAT4: return CommandType::ANALOG;
//		case Device::CHARACTER: return CommandType::ON_OFF;
		default:
			return 0; // undefined ....
		}
	}

  // NOTE: On change this, my need change: OpenDeviceClass::onSensorChanged

	const static uint8_t MAX_ANALOG_VALUE = 255;

	uint8_t id;
	uint16_t pin;
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

	// Listeners
	DeviceListener changeListener;
	DeviceListener syncListerner;

	// Advanced.
	IOExtender* ioExtender;
	ValueFilter* filter;

	Device();
	Device(uint16_t ipin);
	Device(uint16_t ipin, DeviceType type);
	Device(uint16_t ipin, DeviceType type, bool sensor);
	Device(uint8_t iid, uint16_t ipin, DeviceType type);
	Device(uint8_t iid, uint16_t ipin, DeviceType type, bool sensor);

	/**
	 * Change value / state of Device
	 * @param sync - sync with server
	 */
	virtual bool setValue(value_t value, bool sync = true);

	void on();

	void off();

	void toggle();

	bool isON() { return currentValue == HIGH; }

	bool isOFF() { return currentValue == LOW; }

	/**
	 * Get current value.
	 */
	virtual value_t getValue();

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

	Device* setFilter(ValueFilter* filter);

	Device* setIOExtender(IOExtender* _extender);

	bool notifyListeners();

	int toString(Print* conn);

private:
	void _init(char* name, uint8_t iid, uint16_t ipin, Device::DeviceType type, bool sensor);

protected:
	int _digitalRead(uint16_t pin);
	int _analogRead(uint16_t pin);
	void _analogWrite(uint16_t pin, int val);
	void _digitalWrite(uint16_t pin, uint8_t val);

};


#endif /* Device_H_ */
