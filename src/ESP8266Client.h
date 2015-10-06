/*
 * ESP8266Client.h
 *
 *  Created on: 05/10/2015
 *      Author: ricardo
 */

#ifndef ESP8266CLIENT_H_
#define ESP8266CLIENT_H_

#include <Arduino.h>
#include <Stream.h>
#include "config.h"
#include "Command.h"
#include "ESP8266WifiConfig.h"
#include "utility/StreamBuffer.h"

class ESP8266Client : public Stream {
public:
	ESP8266Client();
	~ESP8266Client() {}

    inline int available(){ return buffer.available(); };
    inline int read(){ return buffer.read(); };
    inline int peek(){ return buffer.peek(); };
    inline void flush(){ buffer.flush(); Serial.println("FLUSH....");};
    virtual size_t write(uint8_t);

    void setData(uint8_t *buffer, const uint16_t len);

private:
    uint8_t _buffer[DATA_BUFFER];
	StreamBuffer buffer;
};

#endif /* ESP8266CLIENT_H_ */
