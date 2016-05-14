/*
 * MQTTWifiConnection.h
 *
 *  Created on: May 14, 2016
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_MQTTWIFICONNECTION_H_
#define LIBRARIES_OPENDEVICE_SRC_MQTTWIFICONNECTION_H_


#include <Arduino.h>
#include <Stream.h>

#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <WiFiUdp.h>
	extern "C" {
		#include "user_interface.h"
	}
#endif

#include <PubSubClient.h>


// NOTE: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "utility/Logger.h"
#include "utility/Timeout.h"
#include "DeviceConnection.h"
#include "WifiConnection.h"
#include "MQTTClient.h"


namespace od {

class MQTTWifiConnection : public WifiConnection {

static MQTTClient* mqttClient;

public:
	MQTTWifiConnection();

	virtual ~MQTTWifiConnection();

	virtual void begin(void);

	virtual bool checkDataAvalible(void);

	static void mqttCallback(char* topic, byte* payload, unsigned int length);

private:

#if defined(_YUN_SERVER_H_)
YunClient ethclient;
#endif

// ESP8266 Standalone
#if defined(ESP8266)
WiFiClient ethclient;
#endif

PubSubClient mqtt;
Timeout mqttTimeout;

	void mqttConnect();

};


} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_MQTTWIFICONNECTION_H_ */
