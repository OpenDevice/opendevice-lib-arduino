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

#ifdef WiFi_h

#include <WifiConnection.h>

// TODO: use list: http://stackoverflow.com/a/27088552/955857
WiFiClient WifiConnection::client;

WifiConnection::WifiConnection()
	: server(DEFAULT_SERVER_PORT) {
	isStartup = true;
}

WifiConnection::~WifiConnection() {

}

void WifiConnection::begin(void){

// Only ESP8266 has softAP
#if defined(ESP8266)

	// TODO: verificar se softAP não foi chamado anteriormente e se ele gera um WL_CONNECTED

	WiFiMode mode = WiFi.getMode();
	if(mode == WIFI_AP || mode == WIFI_AP_STA){
		WiFi.softAP(Config.moduleName);
		Logger.debug("SoftAP", "OK");
	}

	// has saved last STA connections settings
	if(wifi_station_get_auto_connect() && isStartup ){
		Logger.debug("Automatic STA Reconnect");
		if(WiFi.waitForConnectResult() != WL_CONNECTED){ // need wait..
			Logger.debug("Automatic STA Reconnect", "FAIL");
		}
	}
#endif


	server.begin();

	Udp.begin(DISCOVERY_PORT);

	Logger.debug("TCPServer", "OK");

    if(Config.debugMode) WiFi.printDiag(Serial);

    // Print the IP address
	Logger.debug("IP STA: ");
    if(Config.debugMode) Serial.println(WiFi.localIP());

    Logger.debug("IP softAP: ");
    if(Config.debugMode) Serial.println(WiFi.softAPIP());

    isStartup = false;
}

bool WifiConnection::checkDataAvalible(void){

#if defined(ESP8266)
	// This is only to debug
	if(STATION_CONNECTING == wifi_station_get_connect_status()){
		Logger.debug("WifiConnection", "reconnecting...");
		if(WiFi.waitForConnectResult() != WL_CONNECTED){ // need wait..
			Logger.debug("STA Reconnect", "FAIL");
		}else{
			Logger.debug("STA Reconnect", "CONNECTED");
		}
	}
#endif

	// entrou em loop
//	if (WiFi.status() != WL_CONNECTED) {
//	    delay(1);
//	    Logger.debug("Connection LOST", "Reconnecting...");
//	    begin();
//	    return false;
//	}


	if (WiFiClient newClient = server.available()) {
		Logger.debug("WifiClient", "connected");
		client = newClient;
		setStream(&client);

		// wait to send data.
		while (!newClient.available()) {
			delay(1);
		}
	}else{

		int count = Udp.parsePacket();
		if ( count ){
			Logger.debug("Udp", "received");
			setStream(&Udp);
			return DeviceConnection::checkDataAvalible();
		}

	}

	if(client.connected()){
		//ODev.setConnected(true);
		return DeviceConnection::checkDataAvalible();
	}else{
		//ODev.setConnected(false);
		return false;
	}

}

void WifiConnection::onMessageReceived(Command cmd){

	if (cmd.type == CommandType::DISCOVERY_REQUEST){
	    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
	    BaseWifiConnection::onMessageReceived(cmd);
	    Udp.endPacket();
	}else{
		BaseWifiConnection::onMessageReceived(cmd);
	}

}

wl_status_t WifiConnection::status(){
	return  WiFi.status();
}

void WifiConnection::restart(){
	// ESP.reset();
}

void WifiConnection::disconnect(bool wifioff){
	WiFi.disconnect(wifioff);
}


void WifiConnection::mode(WiFiMode mode){
	WiFi.mode(mode);
}

bool WifiConnection::begin(const char* ssid, const char *passphrase){

	bool s = WiFi.begin(ssid, passphrase);

	if(!s) Logger.debug("Connect", "Fail");

	return s;
}

char* WifiConnection::getIP(){
	IPAddress addr = WiFi.localIP();

	if(!addr || addr[0] == '0' || addr[0] == 0){
		addr = WiFi.softAPIP();
	}

	sprintf(ipaddress, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);

	return ipaddress;
}

#endif
