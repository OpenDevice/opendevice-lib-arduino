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

 #ifdef ESP8266
  	#include <ESP8266WiFi.h>
 #endif

#ifdef WiFi_h

#include "BaseWifiConnection.h"

BaseWifiConnection::BaseWifiConnection() {

}

BaseWifiConnection::~BaseWifiConnection() {
}



void BaseWifiConnection::onMessageReceived(Command cmd){

	//  Format: /type/id/ctype/uri/passwd
	if (cmd.type == CommandType::CONNECTION_ADD) {

		#if ENABLE_REMOTE_WIFI_SETUP

		Logger.debug("Connect ADD", "WIFI");

		uint8_t ctype = readInt();

		if (ctype == 3) { // Type: WIFI
			String ssid = readString();
			String passwd = readString();
			Logger.debug("Connect to:", ssid.c_str());
			Logger.debug("Using Pass:", passwd.c_str());

			disconnect(false);

			mode(WIFI_AP_STA); // will reset module (if is nor current mode)

			begin(ssid.c_str(), passwd.c_str());

			unsigned long start = millis();
			Serial.print("waitForConnected...");
			waitForConnected(20000);
			Serial.println();
			Serial.print("delay : ");
			Serial.println(millis() - start, DEC);

			begin(); // configure services

			start = millis();
			Serial.print("waitForClient...");
			waitForClient(10000);
			Serial.print("delay : ");
			Serial.println(millis() - start, DEC);

			// Send Back IP;
			doStart();
			print(CommandType::CONNECTION_ADD_RESPONSE);
			doToken();
			print(cmd.id);
			doToken();

			if (status() == WL_CONNECTED) {
				print(ResponseStatus::SUCCESS);
				doToken();
				print(getIP());
				doToken();
			} else {
				print(ResponseStatus::INTERNAL_ERROR);
				doToken();
				print(0);
				doToken();
			}
			doEnd();
		}
		#endif
	} else if (cmd.type == CommandType::DISCOVERY_REQUEST) {
		sendDiscoveryResponse();
		delay(1000);
		sendDiscoveryResponse();
	} else {
		DeviceConnection::onMessageReceived(cmd);
	}

}

void BaseWifiConnection::sendDiscoveryResponse(){
	doStart();
	print(CommandType::DISCOVERY_RESPONSE);
	doToken();
	print(cmd.id);
	doToken();
	print(Config.moduleName);
	doToken();
	print(Device::BOARD); // ModuleType: NODE
	doToken();
	print(Config.devicesLength);
	doToken();
	print(DEFAULT_SERVER_PORT);
	doToken();
	doEnd();
}

bool BaseWifiConnection::waitForClient(uint32_t timeout)
{
    unsigned long start = millis();
    while (millis() - start < timeout) {
    	if(checkDataAvalible()) return true;
    }

    return false;
}

bool BaseWifiConnection::waitForConnected(uint32_t timeout)
{
    unsigned long start = millis();
    while (millis() - start < timeout) {
    	if(status() == WL_CONNECTED) return true;
    	delay(500);
    	if(Config.debugMode) Serial.print(".");
    }

    return false;
}

#endif
