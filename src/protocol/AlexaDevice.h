/*
 * AlexaDevice.h
 *
 *  Created on: 10 de out de 2017
 *      Author: Ricardo JL Rufino
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXADEVICE_H_
#define LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXADEVICE_H_

#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

#include "Device.h"
#include "utility/Logger.h"

#define ALEXA_START_PORT 8000

namespace od {

class AlexaDevice {
private:
    ESP8266WebServer* server;
    Device* device;
    WiFiUDP UDP;
    String serial;
    String persistent_uuid;
    unsigned int localPort;


    void startWebServer();
    void handleEventservice();
    void handleUpnpControl();
    void handleRoot();
    void handleSetupXml();

public:
	AlexaDevice(Device* device, unsigned int index);
	virtual ~AlexaDevice();

	void loop();
	String getAlexaInvokeName();
	void respondToSearch(IPAddress& senderIP, unsigned int senderPort);
};

} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXADEVICE_H_ */

#endif
