#include "../dependencies.h"

#ifdef __ESP8266_H__

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
		#if DEBUG_CON
		Logger.debug("ESP8266Client::write", String((char*) buffer._buffer));
		#endif
		ESP->send(id, buffer._buffer, buffer.current_length());
	}
	return ret;
}

#endif
