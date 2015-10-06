/*
 * ESP8266Client.cpp
 *
 *  Created on: 05/10/2015
 *      Author: ricardo
 */

#include <ESP8266Client.h>

ESP8266Client::ESP8266Client(): buffer(_buffer , DATA_BUFFER) {

}


void ESP8266Client::setData(uint8_t *data, const uint16_t len){
	buffer.flush();
	for(uint32_t i = 0; i < len; i++) {
		buffer.write(data[i]);
	}
}

size_t ESP8266Client::write(uint8_t v){
	size_t ret = buffer.write(v);
	if(v == Command::ACK_BIT){
		WiFi.ESP->send(0, _buffer, buffer.current_length());
		buffer.flush();
		Serial.print("##write:");
		Serial.println(String((char*)_buffer));
	}
	return ret;
}
