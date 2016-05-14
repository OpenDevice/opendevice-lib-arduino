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

#ifndef OPENDEVICE_MQTTConnection
#define OPENDEVICE_MQTTConnection

/*
 * This MQTT implementation is designed to run both on Ethernet and Wifi (ESP8266)
 *
 * Author: Ricardo JL Rufino
 */
#include <Arduino.h>
#include <Stream.h>

#include <PubSubClient.h>

// NOTE: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "utility/Logger.h"
#include "DeviceConnection.h"
#include "MQTTClient.h"

using namespace od;

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS MQTTClient

#if defined(_YUN_SERVER_H_)
static YunClient ethclient;
#endif

// ESP8266 Standalone
#if defined(ESP8266)
static WiFiClient ethclient;
#endif

static PubSubClient mqtt(ethclient);
static MQTTClient mqttClient(mqtt);

static void pubSubClientReconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
	String clientID = String(Config.appID);
	clientID+= "/";
	clientID+= Config.moduleName;

	String subscribe = String(Config.appID);
	subscribe+= "/in/";
	subscribe+= Config.moduleName;

    Serial.print("Connecting...");
    // Attempt to connect
    if (mqtt.connect(clientID.c_str())) { /// ---> NAME.
      Serial.println("connected");
      mqtt.subscribe(subscribe.c_str());
    } else {
      Serial.print("failed (#");
      Serial.print(mqtt.state());
      Serial.println(")");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

static void pubSubClientCallback(char* topic, byte* payload, unsigned int length) {
	mqttClient.setData(payload, length);
}


/** This method is called automatically by the OpenDevice when run: ODev.begin() */
static void custom_connection_begin(){
	Logger.debug("MQTT", "BEGIN");
	mqtt.setServer(Config.server, 1883);
	mqtt.setCallback(pubSubClientCallback);
#if defined(_YUN_SERVER_H_)
	Bridge.begin();
#endif
}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
static MQTTClient custom_connection_loop(DeviceConnection *conn){

	  if (!mqtt.connected()) {
		  pubSubClientReconnect();
	  }

	  mqtt.loop();

	  return mqttClient;

}

#endif


