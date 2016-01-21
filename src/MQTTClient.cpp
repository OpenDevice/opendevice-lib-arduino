/*
 * MQTTClient.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ricardo
 */

#include "../dependencies.h"

#ifdef PubSubClient_h

#include <MQTTClient.h>

namespace od {

MQTTClient::MQTTClient(PubSubClient& mqtt) : StreamBuffer(_buffer, DATA_BUFFER) {
	this->mqtt = &mqtt;
	topic = String(Config.appID);
	topic += "/out";
}

MQTTClient::~MQTTClient() {
}

void MQTTClient::setData(uint8_t *data, const uint16_t len){
	flush();
	for(uint32_t i = 0; i < len; i++) {
		StreamBuffer::write(data[i]);
	}
}

// Write to buffer and wait for ACK to publish
size_t MQTTClient::write(uint8_t v){
	if(v == Command::ACK_BIT){ // don't write ACK
		#if DEBUG_CON
		Serial.print("Write:");
		Serial.println((const char *) StreamBuffer::_buffer);
		#endif
		mqtt->publish(topic.c_str(), (const char *) StreamBuffer::_buffer);
		return 1;
	}else{ // Write to buffer
		return StreamBuffer::write(v);
	}

}


} /* namespace od */

#endif
