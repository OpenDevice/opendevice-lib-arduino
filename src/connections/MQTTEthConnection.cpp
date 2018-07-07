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

StreamBuffer* MQTTEthConnection::buffer;
bool MQTTEthConnection::received = false;

MQTTEthConnection::MQTTEthConnection(Client& client):
		mqtt(client),
		mqttTimeout(RECONNECT_TIMEOUT){

		buffer = new StreamBuffer(_buffer, DATA_BUFFER);

}

MQTTEthConnection::~MQTTEthConnection() {

}

void MQTTEthConnection::begin(){

	Logger.debug("MQTT", "BEGIN");
	mqtt.setServer(Config.server, MQTT_PORT);
	mqtt.setCallback(mqttCallback);

	//  Publish topic
	topic = String(Config.appID);
	topic += "/out/";
	topic += Config.moduleName;

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

		if(received){
			setStream(buffer);
			DeviceConnection::checkDataAvalible();
			received = false;
		}

	}else{ // TCP SERVER...
//		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
//		Config.keepAlive = true; // on raw TCP is  required
//		return WifiConnection::checkDataAvalible();
		if(!mqttTimeout.isEnabled()) mqttTimeout.enable();
		return false;
	}

}

void MQTTEthConnection::mqttCallback(char* topic, byte* payload, unsigned int length){
	buffer->flush();
	received = true;
	for (uint32_t i = 0; i < length; i++) {
		buffer->write(payload[i]);
	}
}

size_t MQTTEthConnection::write(uint8_t v){
	if(v == Command::ACK_BIT){ // don't write ACK
		#if DEBUG_CON
		LOG_DEBUG("MQTT SEND", (const char *) buffer->_buffer);
		#endif
		mqtt.publish(topic.c_str(), (const char *) buffer->_buffer);
		flush();
		return 1;
	}else{ // Write to buffer
		return buffer->write(v);
	}
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
