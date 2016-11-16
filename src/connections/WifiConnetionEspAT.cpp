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

#ifdef __ESP8266AT_H__

#include "WifiConnetionEspAT.h"

WifiConnetionEspAT::WifiConnetionEspAT()
	: statusTimeout(20000),
	  tcpTimeout(20000){
	_status = WL_DISCONNECTED;
	_statusTcp = WL_DISCONNECTED;
	_mode = WIFI_AP_STA;
	ESP = 0;
	clientID = 0;
	softAPEnabled = false;
	;
	memset(ipaddress, 0, 15);
}

WifiConnetionEspAT::~WifiConnetionEspAT() {

}

void WifiConnetionEspAT::init(ESP8266 *impl){
	ESP = impl;
}

void WifiConnetionEspAT::begin(){
	tcpTimeout.enable();
	statusTimeout.enable();
	memset(ipaddress, 0, 15);

	if((_mode == WIFI_AP || _mode == WIFI_AP_STA) && !softAPEnabled /* => Avoid duplicate config*/ ){
		softAP(Config.moduleName, ""); // no password
	}

	bool status = ESP->enableMUX();

#if DEBUG_SETUP
	Logger.debug("MUX", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
#endif

	status = ESP->startTCPServer(DEFAULT_SERVER_PORT);

#if DEBUG_SETUP
	Logger.debug("TCP", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
#endif

	if(status) ESP->setTCPServerTimeout(20000);

	if(status){
		_statusTcp = WL_CONNECTED;
		delay(500);
		getIP(); // required.
		Logger.debug("IP", ipaddress);
		ESP->registerUDP(DISCOVERY_ID, "", DISCOVERY_PORT); // enable discovery port
	}else{
		_statusTcp = WL_CONNECT_FAILED;
	}

	DeviceConnection::begin();

#if DEBUG_SETUP
	Logger.debug("Begin", (status ? "OK" : "FAIL")); // FIXME: use F("...") to reduce memory.
#endif

}

bool WifiConnetionEspAT::checkDataAvalible(void){

	// TODO: talvez seja interessante implantar o sistema de reconexao caso não tenha no firmware AT
	// NOTA: os firmwares já tem implementado , mais é bem ruim a interação com ele.

	// Used to check if has reseted
	//if(tcpTimeout.expired() && statusTcp(true) == WL_CONNECTION_LOST){
	//	Logger.debug(">>> CONNECTION_LOST <<<");
	//	begin(); // setup configurations
	//}

	uint8_t mux_id;
	uint32_t len = ESP->recv(&mux_id, (uint8_t*)_buffer, DATA_BUFFER, 100); // 100ms (TODO: make slow the rest of code)
	if (len > 0) {

		tcpTimeout.reset(); //TODO: o firmware ja tem um timeout interno.

		for (int i = 0 ; i < len; i++){
			store(_buffer[i]);
		}

		clientID = mux_id;

		#if DEBUG_CON
			Serial.print("Received from [");
			Serial.print(mux_id, DEC);
			Serial.print("] -> ");
			for(uint32_t i = 0; i < len; i++) {
				Serial.print((char)_buffer[i]);
			}
			Serial.print("\r\n");
		#endif

		uint8_t type = parseInt();
		parseCommand(type);
		flush();

//		setStream(&client);
//
//		uint8_t ret = DeviceConnection::checkDataAvalible();
//
//		// está demorando para retornar nos comandos normais...
//		Serial.print("ret:");
//		Serial.println(ret);

		return true;
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
#if DEBUG_SETUP
	Logger.debug("Disconnecting...");
#endif
	ESP->leaveAP();
	memset(ipaddress, 0, 15);
	_status = WL_DISCONNECTED;
	_statusTcp = WL_DISCONNECTED;
}

/**
 * Connect to AP
 */
bool WifiConnetionEspAT::begin(const char* ssid, const char *passphrase){

	memset(ipaddress, 0, 15);

	bool status = ESP->joinAP(ssid, passphrase);

#if DEBUG_SETUP
	Logger.debug("Join AP : ", (status ? "OK" : "FAIL"));
#endif

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

	bool status = ESP->enableDHCP(0);

#if DEBUG_SETUP
	Logger.debug("DHCP", (status ? "OK" : "FAIL"));
#endif

	status = ESP->setSoftAPParam(ssid, passphrase, channel, enc);

#if DEBUG_SETUP
	Logger.debug("softAP", (status ? "OK" : "FAIL"));
#endif

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

void WifiConnetionEspAT::doStart(){
	flush();
	write(START_BIT);
}

void WifiConnetionEspAT::doEnd(){
	Serial.print(">>");
	Serial.println((char*)_buffer);

	write('\n');write('\r');
	ESP->send(clientID, (uint8_t*)_buffer, _endOffset);
	flush();
}


size_t WifiConnetionEspAT::write(uint8_t b){
	store(b);
}

WifiConnetionEspAT WiFi;

#endif
