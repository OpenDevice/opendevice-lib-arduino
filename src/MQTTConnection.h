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
 * EthernetServerConnection.h
 * This implementation is designed to run both the Ethernet Arduino native library when the "arduino_uip" used to work with the ENC28J60 module.<br/>
 * This include will be used automatically when the Ethernet or UIPEthernet library is declared in main sketch
 *
 * Created on: 22/06/2014
 * Update: 23/02/2015
 * Author: Ricardo JL Rufino
 */
#include <Arduino.h>
#include <Stream.h>

#include <PubSubClient.h>

// NOTE: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "DeviceConnection.h"
#include "MQTTClient.h"

using namespace od;

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS MQTTClient

YunClient ethclient;
PubSubClient mqtt(ethclient);
MQTTClient mqttClient(mqtt);

void pubSubClientReconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(clientID.c_str())) { /// ---> NAME.
      Serial.println("connected");
      mqtt.subscribe(subscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void pubSubClientCallback(char* topic, byte* payload, unsigned int length) {
	mqttClient.setData(payload, length);
}


/** This method is called automatically by the OpenDevice when run: ODev.begin() */
void custom_connection_begin(){
	mqtt.setServer(Config.server, 1883);
	mqtt.setCallback(pubSubClientCallback);
	Bridge.begin();
}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
MQTTClient custom_connection_loop(DeviceConnection *conn){

	  if (!mqtt.connected()) {
		  pubSubClientReconnect();
	  }

	  mqtt.loop();

	  return mqttClient;

}


