/*
 * MQTTWifiConnection.cpp
 *
 *  Created on: May 14, 2016
 *      Author: ricardo
 */

#ifdef ESP8266
 	#include <ESP8266WiFi.h>
#endif

#ifdef WiFi_h

#include "MQTTWifiConnection.h"

namespace od {

MQTTClient* MQTTWifiConnection::mqttClient;

MQTTWifiConnection::MQTTWifiConnection(): mqtt(ethclient), mqttTimeout(RECONNECT_TIMEOUT) {
	mqttClient = new MQTTClient(mqtt, _buffer);
	connected = false; // for WIFI state
}

MQTTWifiConnection::~MQTTWifiConnection() {
}

void MQTTWifiConnection::begin(){
	 WifiConnection::begin();
	 Logger.debug("MQTT", "BEGIN");
	 mqtt.setServer(Config.server, MQTT_PORT);
	 mqtt.setCallback(mqttCallback);
	 mqttClient->begin();
	 mqttConnect();
}

void MQTTWifiConnection::disconnect(){
	if (!mqtt.connected()) mqtt.disconnect();
}


bool MQTTWifiConnection::checkDataAvalible(void){

	// Monitor Wifi State
	if (WiFi.status() != WL_CONNECTED) {
		hasWiFi = false;
	    return false;
	}

	// This is a reconnection
	if(hasWiFi == false){
		reconnectionsCount++;
		hasWiFi = true;
		mqttTimeout.disable(); // ignore timeout in first connection
		Logger.debug("WiFi - Reconnected", reconnectionsCount);
		Logger.debug("Got IP", WiFi.localIP());
	}


	// Reconnect MQTT if OFFLINE and not have Client (TcpServer)
	if (!mqtt.connected() && (mqttTimeout.expired() || !mqttTimeout.isEnabled()) && !WifiConnection::client.connected()) {
		if (hasWiFi || WiFi.getMode() == WIFI_AP) {
			mqttConnect();
		}
	}

	if (mqtt.connected()) {
		Config.keepAlive = false; // on MQTT is not required
		connected = true;
		mqtt.loop();
		mqttTimeout.disable();
		setStream(mqttClient);
		return DeviceConnection::checkDataAvalible();
	} else { // TCP SERVER...
		if (!mqttTimeout.isEnabled())
			mqttTimeout.enable();
		Config.keepAlive = true; // on raw TCP is  required
		connected = false;
		return WifiConnection::checkDataAvalible();
	}

}

void MQTTWifiConnection::mqttCallback(char* topic, byte* payload, unsigned int length){
	mqttClient->setData(payload, length);
}

void MQTTWifiConnection::mqttConnect(){

	if (WiFi.status() != WL_CONNECTED) {
		return;
	}

	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

	Logger.debug("MQTT", "Connecting... ");

	// Attempt to connect
	if (mqtt.connect(clientID.c_str(), Config.appID, "*")) {
	  Logger.debug("MQTT", "[connected]");
	  mqtt.subscribe(subscribe.c_str());
	} else {
	  Logger.debug("MQTT <Fail>", mqtt.state());
	  mqttTimeout.reset();
	}
	
}

} /* namespace od */

#endif
