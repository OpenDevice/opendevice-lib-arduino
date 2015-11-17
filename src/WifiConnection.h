/*
 * WifiConnection.h
 *
 *  Created on: 07/11/2015
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_
#define LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_


#include <IPAddress.h>

#include "config.h"
#include "utility/Logger.h"
#include "DeviceConnection.h"
#include "BaseWifiConnection.h"


// FIXME: detect correct version of WIFI
#ifdef ESP8266
	#include <ESP8266WiFi.h>
	#include <WiFiUdp.h>
	extern "C" {
		#include "user_interface.h"
	}
#endif


using namespace od;


class WifiConnection : public BaseWifiConnection {

static WiFiClient client;

public:
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

private:
	WiFiServer server;
	boolean isStartup;
	WiFiUDP Udp; // A UDP instance (discovery service)

};

#endif /* LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_ */
