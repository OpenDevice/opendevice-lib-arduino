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

#include <avr/eeprom.h>

#ifndef __cplusplus
#error A C++ compiler is required!
#endif

// #include <EEPROM.h>

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
#define DEFAULT_BAUD 9600
#define ENABLE_DHCP 1  /* if you need save flash memory disable this
                          Another important config to save flash memory is disable UDP of UIPEthernet (UIPEthernet/utility/uipethernet-conf.h) */

namespace od {

	const char server[] = "api.opendevice.io";

	typedef struct{
		bool saved =  false;
		uint8_t id[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		uint8_t ip[4] = { 0, 0, 0, 0 };
		uint8_t devicesLength = 0;
		int devicesStart = 0;
		int devicesEnd = 0;

		void load(){
			eeprom_read_block((void*)this, (void*)0, sizeof(this));
		}

		void save(){
			saved = true;
			eeprom_write_block((void*)this, (void*)0, sizeof(this));
		}

	} ConfigClass;

	extern ConfigClass Config;

	static void loadConfig(){
		eeprom_read_block((void*)&Config, (void*)0, sizeof(Config));
	}
	static void saveConfig(){
		Config.saved = true;
		eeprom_write_block((void*)&Config, (void*)0, sizeof(Config));
	}
}


#endif /* OPENDEVICE_CONFIG_H_ */
