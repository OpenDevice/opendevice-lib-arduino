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


namespace CommandType {
	enum CommandType {
		ON_OFF 					= 1, // Indicates that the values are 0 or 1 (HIGH or LOW)
		ANALOG 					= 2,
		NUMERIC		 			= 3, // Similar to ANALOG , but launches the event 'onChange' every time a reading is taken
		GPIO_DIGITAL 			= 4, // Commands sent directly to the pins (digitalWrite)
		GPIO_ANALOG 			= 5, //  Commands sent directly to the pins (analogWrite)
		INFRA_RED 				= 6,

		DEVICE_COMMAND_RESPONSE = 10, // Response to commands like: ON_OFF, POWER_LEVEL, INFRA RED
		// ---
	    PING                    = 20,
	    PING_RESPONSE           = 21,
	    DISCOVERY_REQUEST       = 22,
	    DISCOVERY_RESPONSE      = 23,
	    MEMORY_REPORT           = 24, // Report the amount of memory = displays the current and maximum).
	    CPU_TEMPERATURE_REPORT  = 25,
	    CPU_USAGE_REPORT        = 26,
		// ---
		GET_DEVICES 			= 30,
		GET_DEVICES_RESPONSE 	= 31,
		CLEAR_DEVICES 			= 32,
		CLEAR_DEVICES_RESPONSE  = 33,
	    DEVICE_ADD 				= 34,
	    DEVICE_ADD_RESPONSE		= 35,
	    DEVICE_DEL 				= 36,
	    DEVICE_DEL_RESPONSE		= 37,
	    CONNECTION_ADD 			= 38,
	    CONNECTION_ADD_RESPONSE	= 39,
	    CONNECTION_DEL 			= 40,
	    CONNECTION_DEL_RESPONSE = 41,
		GET_CONNECTIONS 		= 42,
		GET_CONNECTIONS_RESPONSE   = 43,
		CLEAR_CONNECTIONS 		   = 34,
		CLEAR_ONNECTIONS_RESPONSE  = 35,
		// ---
		USER_COMMAND 			= 99
	};
}

namespace ResponseStatus {
	enum ResponseStatus {
		SUCCESS 			= 200,
		NOT_FOUND 			= 404,
		BAD_REQUEST 		= 400,
		UNAUTHORIZED 		= 401,
		PERMISSION_DENIED 	= 550,
		FORBIDDEN 			= 403,
		INTERNAL_ERROR 		= 500,
		NOT_IMPLEMENTED 	= 501,
		BUFFER_OVERFLOW 	= 502
	};
}

/*namespace SersorType {
	enum SersorType {
		SWITCH = 1, TOOGLE = 2, INFRA_RED = 3,
	};
}*/

struct Command {

	static const uint8_t START_BIT = '/';
	static const uint8_t SEPARATOR = '/';
	static const uint8_t ARRAY_SEPARATOR = ',';
	static const uint8_t ACK_BIT = '\r';

	Command():type(0),id(0),deviceID(0),value(0),length(0){ }

	uint8_t type; // values of CommandType
	uint8_t id;
	uint8_t deviceID;
	unsigned long value;
	uint8_t length;


	static bool isDeviceCommand( uint8_t type ) {
		if (type == 0) return false;

		switch (type) {
		case CommandType::ON_OFF: return true;
		case CommandType::ANALOG: return true;
		default:
			return false;
		}
	}

	static bool isSimpleCommand( uint8_t type ) {
			if (type == 0) return false;
			switch (type) {
			case CommandType::DISCOVERY_REQUEST: return true;
			case CommandType::PING: return true;
			case CommandType::PING_RESPONSE: return true;
			default:
				return false;
			}
		}
};

#endif /* HEADER */

