/*
 * MQTTClient.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ricardo
 */

// #ifdef PubSubClient_h

#include "MQTTClient.h"

namespace od {

MQTTClient::MQTTClient(PubSubClient& mqtt, uint8_t * _buffer) : StreamBuffer(_buffer, DATA_BUFFER) {
	this->mqtt = &mqtt;
}

MQTTClient::~MQTTClient() {
}

void MQTTClient::begin(){
	topic = String(Config.appID);
	topic += "/out/";
	topic += Config.moduleName;
}

void MQTTClient::setData(uint8_t *data, const uint16_t len){
	flush();
	for(uint32_t i = 0; i < len; i++) {
		StreamBuffer::write(data[i]);
	}
//	#if DEBUG_CON
	Serial.print("MQTT RECV: ");
	Serial.println((const char *) StreamBuffer::_buffer);
//	#endif
}

// Write to buffer and wait for ACK to publish
size_t MQTTClient::write(uint8_t v){
	if(v == Command::ACK_BIT){ // don't write ACK
//		#if DEBUG_CON
		Serial.print("MQTT SEND: ");
		Serial.println((const char *) StreamBuffer::_buffer);
//		#endif
		mqtt->publish(topic.c_str(), (const char *) StreamBuffer::_buffer);
		flush();
		return 1;
	}else{ // Write to buffer
		return StreamBuffer::write(v);
	}

}


} /* namespace od */

// #endif
