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

#include "../dependencies.h"

//#ifdef __ESP8266_H__

#include <ESP8266ServerConnection.h>

ESP8266ServerConnection::ESP8266ServerConnection() {
	// TODO Auto-generated constructor stub

}

ESP8266ServerConnection::~ESP8266ServerConnection() {
	// TODO Auto-generated destructor stub
}


void ESP8266ServerConnection::begin(){

	bool status = ESP->enableMUX();

	#if DEBUG_CON
		Logger.debug("enableMUX", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	#endif

	status = ESP->startTCPServer(DEFAULT_SERVER_PORT);

	#if DEBUG_CON
		Logger.debug("TCP Server", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	#endif

	if(status) ESP->setTCPServerTimeout(10000);

	#if DEBUG_CON
		Logger.debug("IP", ESP->getLocalIP());
		Logger.debug("IPStatus", ESP->getIPStatus());
	#endif

	Logger.debug("ESP8266Server.Begin", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	DeviceConnection::begin();
}

void ESP8266ServerConnection::init(ESP8266 *impl){
	ESP = impl;
	client.ESP = impl;
}

bool ESP8266ServerConnection::checkDataAvalible(){

	uint8_t mux_id;
	uint32_t len = ESP->recv(&mux_id, buffer, sizeof(buffer), 100);
	if (len > 0) {
		client.setData(buffer, len);
		client.id = mux_id;

		#if DEBUG_CON
			Serial.print("Received from [");
			Serial.print(mux_id, DEC);
			Serial.print("] -> ");
			for(uint32_t i = 0; i < len; i++) {
				Serial.print((char)buffer[i]);
			}
			Serial.print("\r\n");
		#endif

		setStream(&client);
	}

	DeviceConnection::checkDataAvalible();
}



void ESP8266ServerConnection::mode(WiFiMode mode){
	if(mode == WIFI_STA) ESP->setOprToStation();
	if(mode == WIFI_AP) ESP->setOprToSoftAP();
	if(mode == WIFI_AP_STA) ESP->setOprToStationSoftAP();
}

/* Set up an open access point
 *
 * param ssid: Pointer to the SSID string.
 */
void ESP8266ServerConnection::softAP(const char* ssid){
	// wifi->
}

/* Set up a WPA2-secured access point
 *
 * param ssid: Pointer to the SSID string.
 * param passphrase: Pointer to passphrase, 8 characters min.
 * param channel: WiFi channel number, 1 - 13.
 * param ssid_hidden: Network cloaking? 0 = broadcast SSID, 1 = hide SSID
 */
void ESP8266ServerConnection::softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden){

}

/**
 * Start Wifi connection
 * if passphrase is set the most secure supported mode will be automatically selected
 * @param ssid const char*          Pointer to the SSID string.
 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
 * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
 * @param channel                   Optional. Channel of AP
 * @return
 */
int ESP8266ServerConnection::begin(const char* ssid, const char *passphrase, int32_t channel, uint8_t bssid[6]){
	return ESP->joinAP(ssid, passphrase);
}

int ESP8266ServerConnection::disconnect(bool wifioff){
	return ESP->leaveAP();
}

ESP8266ServerConnection WiFi;

//#endif
