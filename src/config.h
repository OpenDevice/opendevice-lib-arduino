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

#ifndef OPENDEVICE_CONFIG_H_
#define OPENDEVICE_CONFIG_H_

#include <Arduino.h>
#include <EEPROM.h>

// #if defined(__COMPILING_AVR_LIBC__)
// #include <avr/eeprom.h>
// #endif

#ifndef __cplusplus
#error A C++ compiler is required!
#endif

// =====================================
// STATIC CONFIGURATION
// =====================================

#ifndef DEBUG
#define DEBUG	1
#endif

#define DEBUG_SETUP	  1 // set 1 to enable (receiving debug)
#define DEBUG_CON	  0 // set 1 to enable (receiving debug)
#define ENABLE_SERIAL     1

#define API_VERSION   "0.5.2" // software version of this library (used in build_defs.h make X.X.X pattern)
#define CONFIG_VERSION "cv1"  // version of config layout
#define CONFIG_START 0        // start address in EEPROM

#define DEFAULT_BAUD 115200
#define DEFAULT_SERVER_PORT 8182	// Used only in server mode to receive socket connections
#define ODEV_OTA_REMOTE_PORT 80			// Port for remote ota updates
#define DISCOVERY_PORT 6142				// UDP port to enable discovery services.
#define KEEP_ALIVE_INTERVAL 30000
#define KEEP_ALIVE_MAX_MISSING 3
#define ENABLE_DEVICE_INTERRUPTION 0
#define ENABLE_SYNC_DEVICEID 1    // Sync DeviceID from server and save on EEPROM.
#define ENABLE_PREFIX_NAME 0      // Add Module name to Devices
#define LOAD_DEVICE_STORAGE 1     // Load deviceID from EEPROM ? (enable in production)
#define SAVE_DEVICE_INTERVAL 5000     // Set 0 to disable save device state interval

#define RECONNECT_TIMEOUT 30000	//ms

// #define ENABLE_REMOTE_WIFI_SETUP 0   // disable to reduce flash usage
#define ENABLE_SSL 0 // disable to reduce flash/memory usage (tested only for MQTT/ESP8266)
#define ENABLE_ALEXA_PROTOCOL 0 // Enable Alexa/AmazonEcho direct integration (ESP8266 Only)
#define ALEXA_MAX_DEVICES 10 // MAX 14

#ifndef ENABLE_DHCP
#define ENABLE_DHCP 1  /* if you need save flash memory disable this
                          Another important config to save flash memory is disable UDP of UIPEthernet (UIPEthernet/utility/uipethernet-conf.h) */
#endif

#define MAX_DEVICE_ID 255

// ---- Low Memory Devices ----------
#if defined(__AVR_ATtinyX313__) || defined(__AVR_ATtinyX4__) || defined(__AVR_ATtinyX5__)
#define DATA_BUFFER  16
#define MAX_DEVICE_NAME  10
#define MAX_LISTENERS 2
#define MAX_DEVICE 5

#define MAX_COMMAND 5 // this is used for user command callbacks
#define MAX_COMMAND_STRLEN 5
#define READING_INTERVAL 100 // sensor reading interval (ms)

// ---- High Memory Devices --------
#elif defined(ESP8266)
#define DATA_BUFFER  256
#define MAX_LISTENERS 5
#define MAX_DEVICE 20
#define MAX_DEVICE_NAME  25
#define MAX_COMMAND 5 // this is used for user command callbacks
#define MAX_COMMAND_STRLEN 14
#define READING_INTERVAL 100 // sensor reading interval (ms)

// ---- Medium Memory Devices --------
#else
#define DATA_BUFFER  128
#define MAX_LISTENERS 5
#define MAX_DEVICE 10
#define MAX_DEVICE_NAME  25
#define MAX_COMMAND 3 // this is used for user command callbacks
#define MAX_COMMAND_STRLEN 14
#define READING_INTERVAL 100 // sensor reading interval (ms)

#endif


/* Define value type for devince */
typedef double value_t;

// May be better use: https://github.com/mrRobot62/Arduino-logging-library OU -VDEBUG(see ESP8266)
enum DebugTarget{
	DEBUG_SERIAL,
	DEBUG_CURRENT
};

enum ConnectionMode{
	CONNECTION_MODE_CLIENT,
	CONNECTION_MODE_SERVER
};


// =====================================
// DYNAMIC CONFIGURATION
// =====================================

namespace od {

  /**
    * OpenDevice dynamic configuration options. (See defaults in config.cpp file).
	*/
	struct ConfigClass{

	  char version[4]; // version of config - used for validation
	  char moduleName[15];
	  char server[25];
	  char appID[36]; // ApiKey
	  byte id[6];  // MAC
	  byte ip[4];
	  uint8_t pinReset;
	  bool debugMode;
	  bool keepAlive;
	  uint8_t debugTarget;
	  uint8_t connectionMode;
	  int8_t devicesLength;
	  uint16_t devices[MAX_DEVICE]; // DeviceIDs
	  value_t devicesState[MAX_DEVICE]; // Devices state

	  void load();
	  void save();
	  void clear();
	  bool check();

	};

	extern ConfigClass Config;
}


#endif /* OPENDEVICE_CONFIG_H_ */

