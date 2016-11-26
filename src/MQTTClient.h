/*
 * MQTTClient.h
 *
 *  Created on: Jan 20, 2016
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_MQTTCLIENT_H_
#define LIBRARIES_OPENDEVICE_SRC_MQTTCLIENT_H_

#include <Arduino.h>
#include <PubSubClient.h>

#include "config.h"
#include "Command.h"
#include "utility/StreamBuffer.h"


namespace od {

class MQTTClient : public StreamBuffer {
public:
	MQTTClient(PubSubClient&, uint8_t * _buffer);
	virtual ~MQTTClient();

    virtual size_t write(uint8_t);
    void setData(uint8_t *buffer, const uint16_t len);
    void begin();
	bool connected(){ return mqtt->connected(); };

private:
    PubSubClient* mqtt;
    String topic;
};

} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_MQTTCLIENT_H_ */
