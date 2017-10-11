/*
 * AlexaProtocol.h
 *
 *  Created on: 10 de out de 2017
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_
#define LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_

#include <WiFiUdp.h>
#include "Device.h"
#include "AlexaDevice.h"

namespace od {

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

#if defined(ENABLE_ALEXA_PROTOCOL)
extern AlexaProtocol Alexa;
#endif

} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_PROTOCOL_ALEXAPROTOCOL_H_ */
