/*
 * MQTTWifiConnection.cpp
 *
 *  Created on: May 14, 2016
 *      Author: ricardo
 */

#include <MQTTWifiConnection.h>

namespace od {

MQTTClient* MQTTWifiConnection::mqttClient;

MQTTWifiConnection::MQTTWifiConnection(): mqtt(ethclient), mqttTimeout(5000) {
	mqttClient = new MQTTClient(mqtt);
}

MQTTWifiConnection::~MQTTWifiConnection() {
}

void MQTTWifiConnection::begin(){
	WifiConnection::begin();
	Logger.debug("MQTT", "BEGIN");
	mqtt.setServer(Config.server, 1883);
	mqtt.setCallback(mqttCallback);
}

bool MQTTWifiConnection::checkDataAvalible(void){

	if (!mqtt.connected() && mqttTimeout.expired()) {
		mqttConnect();
	}

	if (mqtt.connected()){
		mqtt.loop();
		mqttTimeout.disable();
		setStream(mqttClient);
		return DeviceConnection::checkDataAvalible();
	}else{
		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
		return WifiConnection::checkDataAvalible(); // TCP SERVER...
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
	if (mqtt.connect(clientID.c_str())) {
	  Serial.println("[connected]");
	  mqtt.subscribe(subscribe.c_str());
	} else {
	  Serial.print("failed (#");
	  Serial.print(mqtt.state());
	  Serial.println(")");
	}

}

} /* namespace od */
