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


#include "../dependencies.h"

#if defined(PubSubClient_h) && !defined(WiFi_h)

#include <MQTTConnection.h>

namespace od {

MQTTClient* MQTTConnection::mqttClient;

MQTTConnection::MQTTConnection(Client& client): mqtt(client), mqttTimeout(5000) {
	mqttClient = new MQTTClient(mqtt);
}

MQTTConnection::~MQTTConnection() {
	// TODO Auto-generated destructor stub
}

void MQTTConnection::begin(){
	Logger.debug("MQTT", "BEGIN");
	mqtt.setServer(Config.server, MQTT_PORT);
	mqtt.setCallback(mqttCallback);
	mqttClient->begin();

#if defined(_YUN_SERVER_H_) || defined(_YUN_CLIENT_H_)
	Bridge.begin();
#endif

	mqttConnect();
}

bool MQTTConnection::checkDataAvalible(void){

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

void MQTTConnection::mqttCallback(char* topic, byte* payload, unsigned int length){
	mqttClient->setData(payload, length);
}

void MQTTConnection::mqttConnect(){

	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

	Logger.debug("MQTT", "connecting... ");

	// Attempt to connect
	if (mqtt.connect(clientID.c_str())) {
	  Logger.debug("MQTT", "[connected]");
	  mqtt.subscribe(subscribe.c_str());
	} else {
	  mqttTimeout.reset();
	  Logger.debug("MQTT <Fail>", mqtt.state());
	}

}

} /* namespace od */

#endif
