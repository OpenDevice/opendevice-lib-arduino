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
 * EspLinkConnection <br/>
 *
 * This implementation allows you to use the ESP8266 only as a WiFI bridge <br/>
 * For this you need to flash the "esp-link" firmware and change the MQTT settings in the esp-link WEB setup.

 *  Created on: 20 de nov de 2016
 *  Author: Ricardp JL Rufino
 */

#ifndef ESPLINKCONNECTION_H_
#define ESPLINKCONNECTION_H_

#include "DeviceConnection.h"
#include "utility/StreamBuffer.h"
#include "utility/Logger.h"
#include <ELClient.h>
#include <ELClientCmd.h>
#include <ELClientMqtt.h>

namespace od {

class EspLinkConnection : public DeviceConnection {
public:
	EspLinkConnection(Stream& serial, Stream& debug);
	EspLinkConnection(Stream& serial);
	EspLinkConnection(ELClient& _client);
	virtual ~EspLinkConnection();

	virtual void begin(void);
	virtual bool checkDataAvalible(void);
	virtual size_t write(uint8_t);

	static void wifiCb(void* response);
	static void mqttConnected(void* response);
	static void mqttDisconnected(void* response);
	static void mqttData(void* response);

private:
	String topic;
	static ELClient* esp;
	static StreamBuffer* buffer;
	static ELClientCmd* cmd;
	static ELClientMqtt* mqtt;
	static bool connected;
	static bool received;

	void init(ELClient* _esp);
};

}

#endif /* ESPLINKCONNECTION_H_ */
