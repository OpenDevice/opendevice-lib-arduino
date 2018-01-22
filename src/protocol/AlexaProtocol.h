/*
 * AlexaProtocol.h
 *
 *  Created on: 10 de out de 2017
 *      Author: Ricardo JL Rufino
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_
#define LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_

#include <config.h>

#if defined(ESP8266)

#include <WiFiUdp.h>
#include "Device.h"
#include "AlexaDevice.h"

namespace od {

/**
 * Emulate a Belkin WeMo Device, natively compatible with Alexa.
 * This code is based on: https://github.com/witnessmenow/esp8266-alexa-wemo-emulator
 */
class AlexaProtocol {
private:
	int numOfDevices;
	bool discoveryEnabled;
	AlexaDevice* devices[ALEXA_MAX_DEVICES] = {};
	IPAddress ipMulti;
	const unsigned int portMulti = 1900;
	char packetBuffer[512];
    WiFiUDP UDP;

public:
	AlexaProtocol();
	virtual ~AlexaProtocol();

    bool begin();
    void loop();
    void enableDiscovery(bool val = true);
    void addDevice(Device* device);
};

extern AlexaProtocol Alexa;

} /* namespace od */

#endif

#endif /* LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_ */
