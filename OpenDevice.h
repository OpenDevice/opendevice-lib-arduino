/*
 * OpenDeviceClass.h
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */

#ifndef OpenDevice_H_
#define OpenDevice_H_

#include <Arduino.h>
#include "Command.h"
#include "DeviceConnection.h"
#include "DeviceManager.h"


class OpenDeviceClass {

private:

	// Internal Listeners..
	// NOTE: Static because: deviceManager->setDefaultListener
	static void onMessageReceived(Command cmd);
	static void sensorChanged(uint8_t id, unsigned long value);

	void notifyReceived(ResponseStatus::ResponseStatus status);
	
	// Utils....
	void freeRam();
	void debugChange(uint8_t id, unsigned long value);


public:
 
	bool autoControl; // Changes in the sensor should affect bonded devices..
	Command lastCMD; // Command received / send.
	bool debugMode;
	uint8_t debugTarget;
	DeviceConnection *deviceConnection;
	DeviceManager *deviceManager;

	OpenDeviceClass();
	// virtual ~OpenDeviceClass();

    /**
	 * Setup using the standard serial port
	 * @param baud - Sets the data rate in bits per second, Values:(300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200)
	 */
	void begin(unsigned long baud);
	void begin(DeviceConnection &deviceConnection);

//	void begin(Stream &serial);

	void loop();
	void sendCommand(Command cmd);
	void debug(const __FlashStringHelper* data);
	void debug(const char str[]);
	#ifdef ARDUINO
	void debug(String& str);
	#endif

	bool addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID);
	bool addSensor(uint8_t pin, Device::DeviceType type);
	bool addDevice(uint8_t pin, Device::DeviceType type);
	bool addDevice(Device& device);
	bool addSensor(Device& device);
};

extern OpenDeviceClass OpenDevice;

#endif /* OpenDevice_H_ */
