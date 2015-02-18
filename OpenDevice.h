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


#ifndef OpenDevice_H_
#define OpenDevice_H_

#include <Arduino.h>
#include "Command.h"
#include "DeviceConnection.h"
#include "Device.h"


/*
 * OpenDeviceClass.h
 *
 *  Created on: 27/06/2014
 *      Author: ricardo
 */
class OpenDeviceClass {

private:

	typedef struct _commandCallback {
				char command[MAX_COMMAND_STRLEN];
				void (*function)();
	} CommandCallback;


	Device* devices[MAX_DEVICE];
	CommandCallback commands[MAX_COMMAND];

	// Debouncing of normal pressing (for Sensor's)
	long time;

	// Internal Listeners..
	// NOTE: Static because: deviceConnection->setDefaultListener
	static void onMessageReceived(Command cmd);
	void onSensorChanged(uint8_t id, unsigned long value);

	void notifyReceived(ResponseStatus::ResponseStatus status);
	
	// Utils....
	void clear(Command cmd);
	void freeRam();
	void debugChange(uint8_t id, unsigned long value);

public:


	bool autoControl; // Changes in the sensor should affect bonded devices..
	bool keepAliveEnabled;
	long keepAliveTime;

	Command lastCMD; // Command received / send.
	bool debugMode;
	uint8_t debugTarget;

	uint8_t deviceLength;
	uint8_t commandsLength;
	DeviceConnection *deviceConnection;

	OpenDeviceClass();
	// virtual ~OpenDeviceClass();

    /**
	 * Setup using the standard serial port
	 * @param baud - Sets the data rate in bits per second, Values:(300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, 115200)
	 */
	void begin(unsigned long baud);

	// FIXME: add SoftSerial void begin(unsigned long baud, uint8_t rxpin, uint8_t txpin);

	void begin(DeviceConnection &deviceConnection);

//	void begin(Stream &serial);

	void loop();
	void checkSensorsStatus();

	/** When enabled OpenDevice will be sending a PING message to connection to inform you that everything is OK. <br/>
	 * Control of the Keep Alive / Ping can be left to the other side of the connection, in this case the "device" would be disabled */
	void enableKeepAlive();

	void enableDebug(uint8_t debugTarget = 0);

	void send(Command cmd);

	/** Create a simple command (using lastCMD buffer)*/
	Command cmd(uint8_t type, uint8_t deviceID = 0, unsigned long value = 0);

	void debug(const __FlashStringHelper* data);
	void debug(const char str[]);
	#ifdef ARDUINO
	void debug(String& str);
	#endif

	bool addSensor(uint8_t pin, Device::DeviceType type, uint8_t targetID);
	bool addSensor(uint8_t pin, Device::DeviceType type);
	bool addSensor(Device& device);

	bool addDevice(uint8_t pin, Device::DeviceType type, bool sensor,uint8_t id);
	bool addDevice(uint8_t pin, Device::DeviceType type);
	bool addDevice(Device& device);

	bool addCommand(const char * name, void (*function)());

	Device* getDevice(uint8_t);
	Device* getDeviceAt(uint8_t);

	void setDefaultListener(void (*pt2Func)(uint8_t, unsigned long));

	void setValue(uint8_t id, unsigned long value);
	void sendToAll(unsigned long value);
};

extern OpenDeviceClass OpenDevice;

#endif /* OpenDevice_H_ */
