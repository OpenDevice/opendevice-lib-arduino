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

// Includes
#include "DeviceConnection.h"

#ifdef __LM4F120H5QR__
	#include "itoa.h" // For Stellaris Lauchpad
#endif


extern "C" {
	#include <stdlib.h>
}



// public methods
DeviceConnection::DeviceConnection() : buffer(_buffer , DATA_BUFFER){
	init();
}

DeviceConnection::DeviceConnection(Stream &stream) :  buffer(_buffer , DATA_BUFFER){
	conn = &stream;
	init();
}


void DeviceConnection::init(){

	readTimeout = 1; // ms
	processing = false;

	defaultListener = NULL;
//	for(int a = 0;a < MAX_LISTENERS;a++){
//		listeners[a] = NULL;
//		listeners_key[a] = NULL;
//	}
}


void DeviceConnection::addListener(uint8_t commandType, CommandListener listener ){
//	if (commandType >= 0 && commandType < MAX_LISTENERS){
//		listeners[listenersLength] = listener;
//		listeners_key[listenersLength] = commandType;
//		listenersLength++;
//	}
}

void DeviceConnection::setDefaultListener(CommandListener listener ){
	defaultListener = listener;
}

void DeviceConnection::removeListener(uint8_t command){

	// Only remove pointers
//	for (uint8_t i = 0; i < listenersLength; ++i) {
//		if(listeners_key[i] == command){
//			listeners_key[i] = NULL;
//			listeners[i] = NULL;
//			break;
//		}
//	}

}

bool DeviceConnection::checkDataAvalible(){
	uint8_t lastByte;
	bool timeout = false;
	// non-blocking reading
	// If the timeout (which is usually very low) occur the loop is finished
	do{

		#if DEBUG_CON
			if(conn->available()){
				Serial.print(F("DB:available="));Serial.println(conn->available());Serial.write(ACK_BIT);
			}
		#endif

		// read available data.
		while(conn->available() > 0){

			lastByte = conn->read();


			#if DEBUG_CON
				Serial.print(F("DB:READ:"));Serial.println(lastByte);Serial.write(ACK_BIT);
			#endif

			// NOTE: Start bit is equals to the SEPARATOR
			if(lastByte == START_BIT && !processing){
				processing = true;
				continue;
			}
			else if(lastByte == ACK_BIT){

				#if DEBUG_CON
					Serial.println(F("DB:END_CMD"));Serial.write(ACK_BIT);
				#endif

				processing = false;
				uint8_t type = buffer.parseInt();
				parseCommand(type);
				flush();
				Serial.print("<flush/end>"); delay(200);

			}else if(processing){

				uint8_t w = buffer.write(lastByte);

				if(!w){
					notifyError(ResponseStatus::BUFFER_OVERFLOW);
					#if DEBUG_CON
					Serial.println(F("DB:BUFFER_OVERFLOW"));Serial.write(ACK_BIT);
					#endif
					return false;
				}

			}
		}
		
		// Wait a bit to read the next byte if not available yet.
		// If the timeout (which is usually very low) occur the loop is finished
		if(processing && conn->available() <= 0 && !timeout){
			if(readTimeout > 0) delay(readTimeout);
			if(conn->available() <= 0){
				Serial.print("timeout"); delay(200);
				timeout = true;
			}
		}
	} while(processing && !timeout);

	return timeout;
}


// Private methods
void DeviceConnection::notifyListeners(Command cmd){

	if(defaultListener) (*defaultListener)(cmd);
	// Notify specific listener
//	for (uint8_t i = 0; i < listenersLength; ++i) {
//		if(listeners_key[i] == cmd.type){ // confirm type
//			(*listeners[i])(cmd);
//			break;
//		}
//	}

}


void DeviceConnection::notifyError(ResponseStatus::ResponseStatus status){
	Command cmd;
	cmd.type = CommandType::DEVICE_COMMAND_RESPONSE;
	cmd.deviceID = 0;
	cmd.id = 0;
	cmd.value = status;
	send(cmd, true);
}

void DeviceConnection::parseCommand(uint8_t type){

	Command cmd;
	cmd.type = type;
	cmd.id = readInt();

	if(Command::isDeviceCommand(type)){
		cmd.deviceID = readInt();
		cmd.value = readLong();
	}

	#if DEBUG_CON || DEBUG_ON_PC
		Serial.print("DB:CMD:");
		Serial.print(cmd.type);Serial.print(";");
		Serial.print(cmd.id);Serial.print(";");
		Serial.print(cmd.deviceID);Serial.print(";");
		Serial.print(cmd.value);Serial.print("");
		Serial.write(ACK_BIT);
	#endif

	notifyListeners(cmd);

	Serial.print("<parse/end>"); delay(200);

}

void DeviceConnection::getBuffer(uint8_t buf[]){

	for(int a = 0;a < buffer.current_length();a++){
		buf[a] = _buffer[a];
	}
}

String DeviceConnection::readString(){ return buffer.readString(); }
int DeviceConnection::readInt(){ return buffer.parseInt(); }
long DeviceConnection::readLong(){ return buffer.parseInt(); }
float DeviceConnection::readFloat(){ return buffer.parseFloat(); }

/**
 * Can read single value list like: [1,2,3,4]
 * If you need to read two different arrays like: [1,2,3];[5,2,3,4] call the method 'readIntValues' twice
 */
int DeviceConnection::readIntValues(int values[], int max = -1){ return buffer.readIntValues(values, max); }
int DeviceConnection::readLongValues(long values[], int max = -1){ return buffer.readLongValues(values, max); }
int DeviceConnection::readFloatValues(float values[], int max = -1){ return buffer.readFloatValues(values, max); }


//int DeviceConnection::getArrayLength()
//{
//	if (bufferCount == 1) return 0; // only a flag and ACK_BIT was sent, not data attached
//	numberOfValues = 1;
//	// find the amount of values we got
//	for (int a=1; a<bufferCount;a++){
//		if (_buffer[a]==SEPARATOR) numberOfValues++;
//	}
//	return numberOfValues;
//}


#if defined(ARDUINO) && ARDUINO >= 100
size_t DeviceConnection::write(uint8_t b){
	if(!conn || processing) return 0;
	return conn->write(b);
}
#else
void DeviceConnection::write(uint8_t b){
	if(!conn || processing) return;
	conn->write(b);
}
#endif
	


void DeviceConnection::doStart(){
	write(START_BIT);
}

void DeviceConnection::doToken(){
	write(SEPARATOR);
}

void DeviceConnection::doEnd(){
	write(ACK_BIT);
}


void DeviceConnection::send(char c ){
	write(START_BIT);
	write(c);
	write(ACK_BIT);
}

void DeviceConnection::send(const char str[]){
	if(!conn || processing) return;
	write(START_BIT);
	conn->write(str);
	write(ACK_BIT);
}

void DeviceConnection::send(long values[], int size){
	if(!conn || processing) return;
	conn->write(START_BIT);
	char vbuffer[3];
	for (int i = 0; i < size; ++i) {
		ltoa(values[i], vbuffer, 10);
		conn->write(vbuffer);
		conn->write(SEPARATOR);
	}
	conn->write(ACK_BIT);
}


void DeviceConnection::send(int values[], int size){
	if(!conn || processing) return;
	conn->write(START_BIT);
	char vbuffer[3]; // 3 digits
	for (int i = 0; i < size; ++i) {
		itoa(values[i], vbuffer, 10);
		conn->write(vbuffer);
		conn->write(SEPARATOR);
	}
	conn->write(ACK_BIT);
}

void DeviceConnection::send(uint8_t n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->write(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(int n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(unsigned int n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(long n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(unsigned long n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(long n, int base){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n, base);
	conn->write(ACK_BIT);
}
void DeviceConnection::send(double n){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->print(n);
	conn->write(ACK_BIT);
}
void DeviceConnection::sendln(void){
	if(!conn || processing) return;
	conn->write(START_BIT);
	conn->println();
	conn->write(ACK_BIT);
}


void DeviceConnection::send(Command cmd, bool complete){
	if(!conn || processing) return;
	Serial.print(F("<send/start>")); delay(200);

	conn->write(START_BIT);
	conn->print(cmd.type);
	conn->write(SEPARATOR);
	conn->print(cmd.id);
	conn->write(SEPARATOR);
	conn->print(cmd.deviceID);
	conn->write(SEPARATOR);
	conn->print(cmd.value);
	if(complete)
		conn->write(ACK_BIT);
	else
		conn->write(SEPARATOR);

	Serial.print(F("<send/end>")); delay(200);
}


bool DeviceConnection::isListEnd(char c){
	return  c==']' || c==')' || c=='}';
}

bool DeviceConnection::isSeparator(char c){
	return c==SEPARATOR || c==',';
}

bool DeviceConnection::isListStart(char c){
	return c=='[' || c=='(' || c=='{';
}

void DeviceConnection::flush(){
	buffer.flush();
}
