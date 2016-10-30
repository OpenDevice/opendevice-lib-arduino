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

#ifndef LIBRARIES_OPENDEVICE_SRC_MQTTCONNECTION_H_
#define LIBRARIES_OPENDEVICE_SRC_MQTTCONNECTION_H_


#if defined(_YUN_SERVER_H_) || defined(_YUN_CLIENT_H_)
#include <Bridge.h>
#include <YunClient.h> // Enable for MQTT/Yun (as MQTT Client)
#endif


#include <PubSubClient.h>

// NOTE: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "utility/Logger.h"
#include "utility/Timeout.h"
#include "DeviceConnection.h"
#include "MQTTClient.h"

#define MQTT_PORT 1883

namespace od {

class MQTTEthConnection: public DeviceConnection {

static MQTTClient* mqttClient;

public:

	MQTTEthConnection(Client& client);

	virtual ~MQTTEthConnection();

	virtual void begin(void);

	virtual bool checkDataAvalible(void);

	static void mqttCallback(char* topic, byte* payload, unsigned int length);

private:

	PubSubClient mqtt;
	Timeout mqttTimeout;
	void mqttConnect();

};

} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_MQTTCONNECTION_H_ */
