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


#ifndef LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_
#define LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_

#include "config.h"
#include "utility/Logger.h"
#include "DeviceConnection.h"
#include "BaseWifiConnection.h"
//#include "utility/RemoteUpdate.h"


// FIXME: detect correct version of WIFI
#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <WiFiUdp.h>
	extern "C" {
		#include "user_interface.h"
	}
#endif


using namespace od;

/*
 * WifiConnection
 *
 *  Created on: 07/11/2015
 *  Author: Ricardo JL Rufino
 */
class WifiConnection : public BaseWifiConnection {

public:

	static WiFiClient client; // Current Client

	WifiConnection();
	virtual ~WifiConnection();

	virtual void begin(void);

	virtual bool checkDataAvalible(void);

	virtual void onMessageReceived(Command cmd);

	virtual wl_status_t status();

	virtual void restart();

	virtual void disconnect(bool wifioff = false);

	virtual void mode(WiFiMode mode);

	virtual bool begin(const char* ssid, const char *passphrase);

	virtual char* getIP();


protected:
	WiFiServer server;
	boolean isStartup;
	WiFiUDP Udp; // A UDP instance (discovery service)

	uint8_t waitForConnectResult();

};

#endif /* LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_ */
