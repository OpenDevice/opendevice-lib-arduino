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

#ifndef LIBRARIES_OPENDEVICE_SRC_ESP8266SERVERCONNECTION_H_
#define LIBRARIES_OPENDEVICE_SRC_ESP8266SERVERCONNECTION_H_

#include <Arduino.h>
#include <Stream.h>
#include <ESP8266.h>

#include "config.h"
#include "Logger.h"
#include "DeviceConnection.h"
#include "ESP8266Client.h"

using namespace od;

enum WiFiMode { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 };

/*
 *  @author Ricardo JL Rufino
 *  @date 01/11/2015
 */
class ESP8266ServerConnection : public DeviceConnection {
public:
    ESP8266 *ESP;

	ESP8266ServerConnection();

	virtual ~ESP8266ServerConnection();

	virtual bool checkDataAvalible(void);

	void begin(void);

    void init(ESP8266 *impl);

    void mode(WiFiMode mode);

    /* Set up an open access point
     *
     * param ssid: Pointer to the SSID string.
     */
    void softAP(const char* ssid);

    /* Set up a WPA2-secured access point
	 *
	 * param ssid: Pointer to the SSID string.
	 * param passphrase: Pointer to passphrase, 8 characters min.
	 * param channel: WiFi channel number, 1 - 13.
	 * param ssid_hidden: Network cloaking? 0 = broadcast SSID, 1 = hide SSID
	 */
	void softAP(const char* ssid, const char* passphrase, int channel = 1, int ssid_hidden = 0);

	/**
	 * Start Wifi connection
	 * if passphrase is set the most secure supported mode will be automatically selected
	 * @param ssid const char*          Pointer to the SSID string.
	 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
	 * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
	 * @param channel                   Optional. Channel of AP
	 * @return
	 */
	int begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, uint8_t bssid[6] = NULL);

	int disconnect(bool wifioff = false);


private:
	ESP8266Client client;
	uint8_t buffer[DATA_BUFFER] = {0};
};

extern ESP8266ServerConnection WiFi;

#endif /* LIBRARIES_OPENDEVICE_SRC_ESP8266SERVERCONNECTION_H_ */
