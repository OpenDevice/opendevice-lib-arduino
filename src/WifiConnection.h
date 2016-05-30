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

#ifdef __ARDUINO_OTA_H
	void enableOTA(){
		
		ArduinoOTA.setHostname(Config.moduleName);

		// TODO: change password
		ArduinoOTA.setPassword((const char *)"123");

		ArduinoOTA.begin();

		ArduinoOTA.onStart([]() {
    		Logger.debug(F("<<OpenDevice OTA Start>>"));
  		});

  		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  		 	Logger.debug("Progress", (progress / (total / 100)));
  		});

		ArduinoOTA.onError([](ota_error_t error) {
			if(Config.debugMode){
			    Serial.printf("Error[%u]: ", error);
		    	if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		    	else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		    	else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		    	else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		    	else if (error == OTA_END_ERROR) Serial.println("End Failed");			
			}
		});
	}
#else
	void enableOTA(){
		// DISABLED
	}	
#endif

#ifdef __ARDUINO_OTA_H
	void checkOTA(){
		ArduinoOTA.handle();
	}
#else
	void checkOTA(){
		// DISABLED
	}	
#endif
};

#endif /* LIBRARIES_OPENDEVICE_SRC_WIFICONNECTION_H_ */
