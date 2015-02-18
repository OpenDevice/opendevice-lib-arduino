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

#ifndef COMMAND_H_
#define COMMAND_H_

#include <inttypes.h>

#define ENABLE_SERIAL 1
#define DEBUG_CON	0 // set 1 to enable (receiving debug)

#define API_VERSION   1 // software version of this library
#define OPENDEVICE_LIBRARY_VERSION 050


#define DATA_BUFFER  160
#define MAX_LISTENERS 10
#define MAX_DEVICE 10
#define MAX_COMMAND 10 // this is used for user command callbacks
#define MAX_COMMAND_STRLEN 14
#define READING_INTERVAL 100 // sensor reading interval (ms)
#define KEEP_ALIVE_INTERVAL 1500

// May be better use: https://github.com/mrRobot62/Arduino-logging-library
enum DebugTarget{
	DEBUG_SERIAL,
	DEBUG_CURRENT
};


namespace CommandType {
	enum CommandType {
		ON_OFF = 1,     // Indicates that the values are 0 or 1 (HIGH or LOW)
		ANALOG = 2,
		ANALOG_REPORT = 3,
		GPIO_DIGITAL = 4,   // Commands sent directly to the pins (digitalWrite)
		GPIO_ANALOG = 5,   //  Commands sent directly to the pins (analogWrite)
		PWM = 6,
		INFRA_RED = 7,
		//
		USER_COMMAND = 9,
		DEVICE_COMMAND_RESPONSE = 10, // Response to commands like: ON_OFF, POWER_LEVEL, INFRA RED
		PING = 20,
		PING_RESPONSE = 21,
		MEMORY_REPORT = 22, //  Reports the current and maximum Memory.
		CPU_TEMPERATURE_REPORT = 23,
		CPU_USAGE_REPORT = 24,
		GET_DEVICES = 30,
		GET_DEVICES_RESPONSE = 31

	};
}

namespace ResponseStatus {
	enum ResponseStatus {
		SUCCESS = 200,
		NOT_FOUND = 404,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		PERMISSION_DENIED = 550,
		FORBIDDEN = 403,
		INTERNAL_ERROR = 500,
		NOT_IMPLEMENTED = 501,
		BUFFER_OVERFLOW = 502
	};
}

/*namespace SersorType {
	enum SersorType {
		SWITCH = 1, TOOGLE = 2, INFRA_RED = 3,
	};
}*/

struct Command {

	static const uint8_t SEPARATOR = ';';
	static const uint8_t ARRAY_SEPARATOR = ',';
	static const uint8_t START_BIT = 18;
	static const uint8_t ACK_BIT = 19;
	static const uint8_t STOP_BIT = 27;

	uint8_t type; // values of CommandType
	uint8_t id;
	uint8_t deviceID;
	unsigned long value;
	size_t length;
};

#endif /* HEADER */

