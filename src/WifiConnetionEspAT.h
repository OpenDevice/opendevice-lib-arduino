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


#ifndef LIBRARIES_OPENDEVICE_SRC_WIFICONNETIONESPAT_H_
#define LIBRARIES_OPENDEVICE_SRC_WIFICONNETIONESPAT_H_

#include <ESP8266AT.h>

#include "config.h"
#include "utility/Timeout.h"
#include "BaseWifiConnection.h"

using namespace od;

/*
 * WifiConnetionEspAT
 *
 *  Created on: 12/11/2015
 *  Author: Ricardo JL Rufino
 */
class WifiConnetionEspAT: public BaseWifiConnection {
public:

	ESP8266 *ESP;

	WifiConnetionEspAT();

	virtual ~WifiConnetionEspAT();

	void init(ESP8266 *impl);

	virtual void begin(void);

	virtual bool checkDataAvalible(void);

	virtual wl_status_t status();

	virtual void restart();

	virtual void disconnect(bool wifioff = false);

	virtual void mode(WiFiMode mode);

	virtual bool begin(const char* ssid, const char *passphrase);

    /* Set up a WPA2-secured access point
     *
     * param ssid: Pointer to the SSID string.
     * param passphrase: Pointer to passphrase, 8 characters min.
     * param channel: WiFi channel number, 1 - 13.
     */
    void softAP(const char* ssid, const char* passphrase, int channel = 1);

	virtual char* getIP();

	virtual size_t write(uint8_t);

	virtual void doStart();

	virtual void doEnd();

private:
	const uint8_t DISCOVERY_ID = 3;
	uint8_t clientID;
//	int stateFlags;
	WiFiMode _mode;
	bool softAPEnabled;
	wl_status_t _status;
	wl_status_t _statusTcp; // used to check if has reseted or wifi lost/reconnected
	Timeout statusTimeout;
	Timeout tcpTimeout;

	wl_status_t statusTcp(bool force = false);

};

extern WifiConnetionEspAT WiFi;

#endif /* LIBRARIES_OPENDEVICE_SRC_WIFICONNETIONESPAT_H_ */
