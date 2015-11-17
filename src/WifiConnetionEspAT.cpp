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

#ifdef __ESP8266AT_H__

#include <WifiConnetionEspAT.h>

WifiConnetionEspAT::WifiConnetionEspAT()
	: statusTimeout(20000),
	  tcpTimeout(20000){
	_status = WL_DISCONNECTED;
	_statusTcp = WL_DISCONNECTED;
	_mode = WIFI_AP_STA;
	ESP = 0;
	softAPEnabled = false;
	;
	memset(ipaddress, 0, 15);
}

WifiConnetionEspAT::~WifiConnetionEspAT() {

}

void WifiConnetionEspAT::init(ESP8266 *impl){
	ESP = impl;
	client.ESP = impl;
}

void WifiConnetionEspAT::begin(){
	tcpTimeout.reset();
	statusTimeout.reset();
	memset(ipaddress, 0, 15);

	if(!softAPEnabled && (_mode == WIFI_AP || _mode == WIFI_AP_STA)){
		softAP(Config.moduleName, ""); // no password
	}

	bool status = ESP->enableMUX();

	#if DEBUG_CON
		Logger.debug("enableMUX", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	#endif

	status = ESP->startTCPServer(DEFAULT_SERVER_PORT);

	if(status) ESP->setTCPServerTimeout(20000);

	#if DEBUG_CON
		Logger.debug("TCP Server", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	#endif

	if(status){
		_statusTcp = WL_CONNECTED;
		delay(500);
		getIP(); // required.
		Logger.debug("IP", ipaddress);
		ESP->registerUDP(DISCOVERY_ID, "", DISCOVERY_PORT); // enable discovery port
	}else{
		_statusTcp = WL_CONNECT_FAILED;
	}

	#if DEBUG_CON
		Logger.debug("ESP8266Server.Begin", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
	#endif

	DeviceConnection::begin();
}

bool WifiConnetionEspAT::checkDataAvalible(void){

	// TODO: talvez seja interessante implantar o sistema de reconexao caso não tenha no firmware AT
	// NOTA: os firmwares já tem implementado , mais é bem ruim a interação com ele.

	// Used to check if has reseted
	if(tcpTimeout.expired() && statusTcp(true) == WL_CONNECTION_LOST){
		Logger.debug(">>> CONNECTION_LOST <<<");
		begin(); // setup configurations
	}

	uint8_t mux_id;
	uint32_t len = ESP->recv(&mux_id, buffer, sizeof(buffer), 100); // 100ms (TODO: make slow the rest of code)
	if (len > 0) {
		client.setData(buffer, len);
		client.id = mux_id;

//				#if DEBUG_CON
			Serial.print("Received from [");
			Serial.print(mux_id, DEC);
			Serial.print("] -> ");
			for(uint32_t i = 0; i < len; i++) {
				Serial.print((char)buffer[i]);
			}
			Serial.print("\r\n");
//				#endif

		setStream(&client);
		return DeviceConnection::checkDataAvalible();
	}else{
		return false;
	}
}

void WifiConnetionEspAT::mode(WiFiMode mode){
	_mode = mode;
	if(mode == WIFI_STA) ESP->setOprToStation();
	if(mode == WIFI_AP) ESP->setOprToSoftAP();
	if(mode == WIFI_AP_STA) ESP->setOprToStationSoftAP();
}

wl_status_t WifiConnetionEspAT::status(){

	if(_status == WL_CONNECTED){
		// check only timeout occurred (for performance reasons)
		if(statusTimeout.expired()){
			_status = (ESP->connected() ? WL_CONNECTED : WL_DISCONNECTED);
		}
	}else{
		_status = (ESP->connected() ? WL_CONNECTED : WL_DISCONNECTED);
	}

	return  _status;

}

wl_status_t WifiConnetionEspAT::statusTcp(bool force){

	// check only timeout occurred (for performance reasons)
	if(tcpTimeout.expired() || force){
		String data = ESP->getIPStatus();
		if (data.indexOf("+CIPSTATUS:3") != -1) { // 3 is ID o discovery service.
			_statusTcp = WL_CONNECTED;
		}else{
			_statusTcp = WL_CONNECTION_LOST;
		}
	}

	return  _statusTcp;
}

void WifiConnetionEspAT::restart(){
	ESP->restart();
}

void WifiConnetionEspAT::disconnect(bool wifioff){
	Logger.debug("Disconnecting...");
	ESP->leaveAP();
	memset(ipaddress, 0, 15);
	_status = WL_DISCONNECTED;
	_statusTcp = WL_DISCONNECTED;
}

bool WifiConnetionEspAT::begin(const char* ssid, const char *passphrase){

	memset(ipaddress, 0, 15);

	bool status = ESP->joinAP(ssid, passphrase);

	if(status) _status = WL_CONNECTED;

	return status;
}

/* Set up a WPA2-secured access point
 *
 * param ssid: Pointer to the SSID string.
 * param passphrase (optional): Pointer to passphrase, 8 characters min.
 * param channel: WiFi channel number, 1 - 13.
 */
void WifiConnetionEspAT::softAP(const char* ssid, const char* passphrase, int channel){
	softAPEnabled = true;
	uint8_t enc = 4; // WPA_WPA2_PSK
	if(!passphrase || strlen(passphrase) == 0 ) enc = 0;
	ESP->setSoftAPParam(ssid, passphrase, channel, enc);
}

char* WifiConnetionEspAT::getIP(){

	if(!ipaddress || ipaddress[0] == '0' || strlen(ipaddress) == 0){
		strcpy(ipaddress, ESP->getLocalIP().c_str());
		if(!ipaddress || ipaddress[0] == '0' || strlen(ipaddress) == 0){
			strcpy(ipaddress, ESP->getSoftAPIP().c_str());
		}
	}

	return ipaddress;
}

WifiConnetionEspAT WiFi;

#endif
