/*
 * DeviceManager.h
 *
 *  Created on: 05/02/2013
 *      Author: Ricardo JL Rufino
 */

#ifndef DeviceManager_H_
#define DeviceManager_H_

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


class DeviceManager {
private:
	Device* devices[MAX_DEVICE];

	// Debouncng of normal pressing (for Sensor's)
	long time;

	void _init();
	bool addDevice(uint8_t pin, Device::DeviceType type, bool sensor,uint8_t id);

	/**
	 * Registered Callback function
	 * Params: pinNumber, newValue
	 */
	void (*callbackPtr)(uint8_t pinNumber, unsigned long newValue);


public:
	int deviceLength;

	DeviceManager();
	// bool addSensor(Sensor* sensor);
	bool addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID);
	bool addDevice(uint8_t pin, Device::DeviceType type);
	bool addDevice(Device& device);
	Device* getDevice(uint8_t);
	Device* getDeviceAt(uint8_t);

	void setDefaultListener(void (*pt2Func)(uint8_t, unsigned long));
	void checkStatus();
	void setValue(uint8_t id, unsigned long value);
	void sendToAll(unsigned long value);
	void init();

};

#endif /* DeviceManager_H_ */
