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
#include <inttypes.h>
#include <string.h>
#include "Stream.h"
#include "Command.h"


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
	Stream * com;
	virtual void init();
private:
	
	// Alias
	static const uint8_t START_BIT = Command::START_BIT;
	static const uint8_t ACK_BIT = Command::ACK_BIT;
	static const uint8_t STOP_BIT = Command::STOP_BIT;
	static const uint8_t SEPARATOR = Command::SEPARATOR;
	const char* SEPARATOR_LIST = ";";


	int numberOfValues;
	uint16_t readTimeout; // time in microseconds to the wait for next char

	CommandListener defaultListener;  			// default listener
	CommandListener listeners[MAX_LISTENERS];   // user listeners
	uint8_t listeners_key[MAX_LISTENERS];       // listeners keys(CommandType).
	int listenersLength;

	// private methods
	void parseCommand(uint8_t type);
	void notifyListeners(Command);
	void notifyError(ResponseStatus::ResponseStatus status);

	int getArrayLength();

	bool isListStart(char c);
	bool isListEnd(char c);
	bool isSeparator(char c);
	int nextEndOffSet();

public: 

	char buffer[DATA_BUFFER];
	uint8_t bufferCount;
	char *bufferPos;
	int bufferOffset;

	// public methods
	DeviceConnection();
	DeviceConnection(Stream &serial);
	
	void flush(void);
	virtual bool checkDataAvalible(void);

	void setDefaultListener(CommandListener);
	void addListener(uint8_t,CommandListener);
	void removeListener(uint8_t);
	int bufferLength(){return bufferCount;} // buffer withouth ACK
	int stringLength(){return bufferCount;} // string without flag but '/0' at the end
	void getBuffer(uint8_t[]);
	
	char * readString();
	int readInt();
	long readLong();
	float readFloat();
	double readDouble();
	int readIntValues(int[], int max = -1);
	void readFloatValues(float[]);
	void readDoubleValues(float[]); // in Arduino double and float are the same
	
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
    	com->write(START_BIT);
    	com->print(arg);
    	com->write(ACK_BIT);
    }

    /** Umanaged send data, must be used with doStart/doToken/doEnd */
    template < class T > void print (T arg){
    	com->print(arg);
    }
	
	static int api_version() { return API_VERSION;}
};


#endif
