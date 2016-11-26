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
 * MQTTConnection.h
 *
 * Enable MQTT for Ethernet or Yun connections
 *
 *  Created on: Aug 12, 2016
 *  Author: Ricardo JL Rufino
 */

#include <connections/MQTTEthConnection.h>

namespace od {

MQTTClient* MQTTEthConnection::mqttClient;

MQTTEthConnection::MQTTEthConnection(Client& client): mqtt(client), mqttTimeout(5000) {
	mqttClient = new MQTTClient(mqtt, _buffer);
}

MQTTEthConnection::~MQTTEthConnection() {

}

void MQTTEthConnection::begin(){

	Logger.debug("MQTT", "BEGIN");
	mqtt.setServer(Config.server, MQTT_PORT);
	mqtt.setCallback(mqttCallback);
	mqttClient->begin();
	mqttConnect();

}

bool MQTTEthConnection::checkDataAvalible(void){

	// Reconnect MQTT if OFFLINE and not have Client (TcpServer)
	if (mqttTimeout.expired() && !mqtt.connected()) {
		mqttConnect();
	}

	if (mqtt.connected()){
		Config.keepAlive = false; // on MQTT is not required
		mqtt.loop();
		mqttTimeout.disable();
		setStream(mqttClient);
		return DeviceConnection::checkDataAvalible();
	}else{ // TCP SERVER...
//		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
//		Config.keepAlive = true; // on raw TCP is  required
//		return WifiConnection::checkDataAvalible();
		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
		return false;
	}

}

void MQTTEthConnection::mqttCallback(char* topic, byte* payload, unsigned int length){
	mqttClient->setData(payload, length);
}

void MQTTEthConnection::mqttConnect(){

	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

	LOG_DEBUG("MQTT", "connecting... ");

	// Attempt to connect
	if (mqtt.connect(clientID.c_str(), Config.appID, "*")) {
	  LOG_DEBUG("MQTT", "[connected]");
	  mqtt.subscribe(subscribe.c_str());
	} else {
	  mqttTimeout.reset();
	  LOG_DEBUG("MQTT <Fail>", mqtt.state());
	}

}

} /* namespace od */

// #endif
