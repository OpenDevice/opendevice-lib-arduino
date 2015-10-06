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
 * This implementation is designed to run both the Ethernet Arduino native library when the "arduino_uip" used to work with the ENC28J60 module.<br/>
 * This include will be used automatically when the Ethernet or UIPEthernet library is declared in main sketch
 *
 * Created on: 22/06/2014
 * Update: 23/02/2015
 * Author: Ricardo JL Rufino
 */
#include <Arduino.h>
#include <Stream.h>
#include <ESP8266.h>


// [[[ALERT]]]: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "DeviceConnection.h"
#include "./utility/StreamBuffer.h"
#include "ESP8266WifiConfig.h" // Make API compatible with native Arduino Wifi Lib
#include "ESP8266Client.h"

using namespace od;

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS ESP8266Client


uint8_t buffer[DATA_BUFFER] = {0};
uint8_t mux_id;
ESP8266Client client;


/** This method is called automatically by the OpenDevice when run: ODev.begin() */
void custom_connection_begin(){
	Serial.begin(9600);
	Serial.println(" - Setup ESP8266");

	//esp = WiFi.ESP;
	if(!WiFi.ESP)  Serial.println("ESP8266 NULL");
	else Serial.println("ESP8266 OK");

//	if (wifi->setOprToStationSoftAP()) {
//		Serial.print("station + softap [ok]\r\n");
//	} else {
//		Serial.print("to station + softap err\r\n");
//	}
//
//	if (wifi->joinAP(SSID, PASSWORD)) {
//		Serial.print("Join AP [success]\r\n");
//		Serial.print("IP: ");
//		Serial.println(wifi->getLocalIP().c_str());
//	} else {
//		Serial.print("Join AP failure\r\n");
//	}

	if (WiFi.ESP->enableMUX()) {
		Serial.print("multiple [OK]\r\n");
	} else {
		Serial.print("multiple err\r\n");
	}

	if (WiFi.ESP->startTCPServer(DEFAULT_SERVER_PORT)) {
		Serial.print("start tcp server [OK]\r\n");
	} else {
		Serial.print("start tcp server err\r\n");
	}

	if (WiFi.ESP->setTCPServerTimeout(10)) {
		Serial.print("set tcp server timout [OK] \r\n");
	} else {
		Serial.print("set tcp server timout err\r\n");
	}

	// ODev.debug("ESP8266 Begin [ok]");
}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
CUSTOM_CONNECTION_CLASS custom_connection_loop(DeviceConnection *conn){

	uint8_t mux_id;
	uint32_t len = WiFi.ESP->recv(&mux_id, buffer, sizeof(buffer), 100);
	if (len > 0) {
		client.setData(buffer, len);
		Serial.print("Received from :");
		Serial.print(mux_id);
		Serial.print("[");
		for(uint32_t i = 0; i < len; i++) {
			Serial.print((char)buffer[i]);
		}
		Serial.print("]\r\n");
	}

	return client;

}

