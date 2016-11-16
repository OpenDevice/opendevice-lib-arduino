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

MQTTWifiConnection::MQTTWifiConnection(): mqtt(ethclient), mqttTimeout(5000) {
	mqttClient = new MQTTClient(mqtt, _buffer);
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

bool MQTTWifiConnection::checkDataAvalible(void){

	// Reconnect MQTT if OFFLINE and not have Client (TcpServer)
	if (!mqtt.connected() && mqttTimeout.expired() &&  !WifiConnection::client.connected()) {
		if(WiFi.status() == WL_CONNECTED || WiFi.getMode() == WIFI_AP){
			mqttConnect();
		}
	}

	if (mqtt.connected()){
		Config.keepAlive = false; // on MQTT is not required
		mqtt.loop();
		mqttTimeout.disable();
		setStream(mqttClient);
		return DeviceConnection::checkDataAvalible();
	}else{ // TCP SERVER...
		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
		Config.keepAlive = true; // on raw TCP is  required
		return WifiConnection::checkDataAvalible();
	}

}

void MQTTWifiConnection::mqttCallback(char* topic, byte* payload, unsigned int length){
	mqttClient->setData(payload, length);
}

void MQTTWifiConnection::mqttConnect(){

	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

	Logger.debug("MQTT", "connecting... ");
	// Attempt to connect
	if (mqtt.connect(clientID.c_str(), Config.appID, "*")) {
	  Logger.debug("MQTT", "[connected]");
	  mqtt.subscribe(subscribe.c_str());
	} else {
	  mqttTimeout.reset();
	  Logger.debug("MQTT <Fail>", mqtt.state());
	}

}

} /* namespace od */

#endif
