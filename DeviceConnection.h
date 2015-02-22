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

#ifndef DeviceConnection_h
#define DeviceConnection_h

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "Command.h"
#include "CommandBuffer.h"

extern "C"
{
  // Definition of the listener function
  typedef void (*CommandListener) (Command);
}

/******************************************************************************
* Definitions
******************************************************************************/

/**
 * Class que implements the application level protocol of the OpenDevice.
 */
class DeviceConnection{


protected:
	virtual void init();
private:
	
	// Alias
	static const uint8_t START_BIT = Command::START_BIT;
	static const uint8_t ACK_BIT = Command::ACK_BIT;
	static const uint8_t SEPARATOR = Command::SEPARATOR;
	const char* SEPARATOR_LIST = ";";

	bool processing;
	char _buffer[DATA_BUFFER];
	uint16_t readTimeout; // time in ms to the wait for end command

	CommandListener defaultListener;  			// default listener
//	CommandListener listeners[MAX_LISTENERS];   // user listeners
//	uint8_t listeners_key[MAX_LISTENERS];       // listeners keys(CommandType).
//	int listenersLength;

	// private methods
	void parseCommand(uint8_t type);
	void notifyListeners(Command);
	void notifyError(ResponseStatus::ResponseStatus status);

	int getArrayLength();

public: 
	Stream  *conn;
	CommandBuffer buffer;

	// public methods
	DeviceConnection();
	DeviceConnection(Stream &serial);
	
	void flush(void);
	virtual bool checkDataAvalible(void);

	void setStream(Stream *stream) { conn = stream; };
	void setDefaultListener(CommandListener);
//	void addListener(uint8_t,CommandListener);
//	void removeListener(uint8_t);
	void getBuffer(uint8_t[]);
	
	inline String readString(){ return buffer.readString(); }
	inline int readInt(){ return buffer.parseInt(); }
	inline long readLong(){ return buffer.parseInt(); }
	inline float readFloat(){ return buffer.parseFloat(); }

	/**
	 * Can read single value list like: [1,2,3,4]
	 * If you need to read two different arrays like: [1,2,3];[5,2,3,4] call the method 'readIntValues' twice
	 */
	inline int readIntValues(int values[], int max){ return buffer.readIntValues(values, max); }
	inline int readLongValues(long values[], int max){ return buffer.readLongValues(values, max); }
	inline int readFloatValues(float values[], int max){ return buffer.readFloatValues(values, max); }
	

	#if defined(ARDUINO) && ARDUINO >= 100
	size_t write(uint8_t);
	#else
	void write(uint8_t);
	#endif
	

	void doStart();
	void doToken();
	void doEnd();

	void send(char);
    void send(const char[]);
    void send(int[], int);
    void send(long values[], int size);
    void send(uint8_t);
    void send(int);
    void send(unsigned int);
    void send(long);
    void send(unsigned long);
    void send(long, int);
    void send(double);
    void sendln(void);
    void send(Command, bool complete = true);

    template < class T > void sendCmdArg (T arg){
    	conn->write(START_BIT);
    	conn->print(arg);
    	conn->write(ACK_BIT);
    }

    /** Umanaged send data, must be used with doStart/doToken/doEnd */
    template < class T > void print (T arg){
    	conn->print(arg);
    }
	
	static int api_version() { return API_VERSION;}
};


#endif
