/*
 * OpenDevice.h
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */

#ifndef OPENDEVICE_H_
#define OPENDEVICE_H_

#include <Arduino.h>
#include "Command.h"
#include "DeviceConnection.h"
#include "DeviceManager.h"

class OpenDevice {

private:

	// Internal Listeners..
	// NOTE: Static because: deviceManager->setDefaultListener
	static void onMessageReceived(Command);
	static void notifyReceived(ResponseStatus::ResponseStatus);
	static void sensorChanged(uint8_t id, uint16_t value);

	// Utils....
	static void freeRam();
	static void debugChange(uint8_t id, uint16_t value);



public:

	static bool autoControl; // Changes in the sensor should affect bonded devices..
	static Command lastCMD; // Command received / send.
	static bool debugMode;
	static uint8_t debugTarget;
	static DeviceConnection *deviceConnection;
	static DeviceManager *deviceManager;

	OpenDevice();
	OpenDevice(DeviceConnection*, DeviceManager*);
	// virtual ~OpenDevice();
	static void begin(DeviceConnection&);
//	static void begin(Stream &serial);

	static void loop();
	static void sendCommand(Command cmd);
	static void debug(const __FlashStringHelper* data);
	static void debug(const char str[]);

	static bool addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID);
	static bool addSensor(uint8_t pin, Device::DeviceType type);
	static bool addDevice(uint8_t pin, Device::DeviceType type);
};

#endif /* OPENDEVICE_H_ */
