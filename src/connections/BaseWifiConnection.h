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

#ifndef LIBRARIES_OPENDEVICE_BASEWIFICONNECTION_H_
#define LIBRARIES_OPENDEVICE_BASEWIFICONNECTION_H_

#include "config.h"
#include "Device.h"
#include "utility/Logger.h"
#include "DeviceConnection.h"

#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <WiFiUdp.h>
	extern "C" {
		#include "user_interface.h"
	}
#endif

using namespace od;

/*
 * BaseWifiConnection
 *
 *  Created on: 08/11/2015
 *  Author: ricardo
 */
class BaseWifiConnection : public DeviceConnection  {
public:

	BaseWifiConnection();

	virtual ~BaseWifiConnection();

	virtual void mode(WiFiMode mode) = 0;

	virtual void restart() = 0;

	virtual void begin(void) = 0;

	virtual bool begin(const char* ssid, const char *passphrase) = 0;

	virtual wl_status_t status() = 0;

	/** Return IP of STA if connected, else return IP of softAP*/
	virtual char* getIP() = 0;

	virtual void disconnect(bool wifioff) = 0;

protected:

	char ipaddress[15];

	virtual void onMessageReceived(Command);

	bool waitForClient(uint32_t timeout);

	bool waitForConnected(uint32_t timeout);

	void sendDiscoveryResponse();

};

#endif /* LIBRARIES_OPENDEVICE_BASEWIFICONNECTION_H_ */
