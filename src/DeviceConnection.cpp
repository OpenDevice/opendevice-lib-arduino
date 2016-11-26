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
DeviceConnection::DeviceConnection(){
	init();
}

DeviceConnection::DeviceConnection(Stream &stream){
	conn = &stream;
	init();
}


void DeviceConnection::begin(){
	// nothing
}

void DeviceConnection::init(){

	readTimeout = 1; // ms
	processing = false;

	defaultListener = NULL;
	_buffer_overflow = false;
	_endOffset = 0;


//	for(int a = 0;a < MAX_LISTENERS;a++){
//		listeners[a] = NULL;
//		listeners_key[a] = NULL;
//	}
}


//void DeviceConnection::addListener(uint8_t commandType, CommandListener listener ){
//	if (commandType >= 0 && commandType < MAX_LISTENERS){
//		listeners[listenersLength] = listener;
//		listeners_key[listenersLength] = commandType;
//		listenersLength++;
//	}
//}

void DeviceConnection::setDefaultListener(CommandListener listener ){
	defaultListener = listener;
}

//void DeviceConnection::removeListener(uint8_t command){

	// Only remove pointers
//	for (uint8_t i = 0; i < listenersLength; ++i) {
//		if(listeners_key[i] == command){
//			listeners_key[i] = NULL;
//			listeners[i] = NULL;
//			break;
//		}
//	}

//}

bool DeviceConnection::checkDataAvalible(){

	if(!conn) return false;

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
	    uint8_t available = 0;
		while( available > 0 || (available = conn->available()) ){

			lastByte = conn->read();
			available--;

			#if DEBUG_CON
				Serial.print(F("DB:READ:"));Serial.println((char)lastByte);
			#endif

			// NOTE: Start bit is equals to the SEPARATOR
			if(lastByte == START_BIT && !processing){
				processing = true;
				//digitalWrite(11,HIGH);
				//digitalWrite(10,HIGH);
				continue;
			}
			else if(lastByte == ACK_BIT){

				#if DEBUG_CON
					Serial.println(F("DB:END_CMD"));
				#endif

				processing = false;

				uint8_t type = parseInt();
				// digitalWrite(11,LOW);
				parseCommand(type);
				

				return true;

			}else if(processing){

				uint8_t w = store(lastByte);
				// digitalWrite(10, !digitalRead(10));

				if(!w){
					notifyError(ResponseStatus::BUFFER_OVERFLOW);
					#if DEBUG_CON
					Serial.println(F("DB:BUFFER_OVERFLOW"));
					#endif
					return false;
				}

			}
		}

		// // Wait a bit to read the next byte if not available yet.
		// // If the timeout (which is usually very low) occur the loop is finished
		// if(processing && !timeout){
		// 	if(readTimeout > 0) delay(readTimeout);
		// 	if(conn->available() <= 0){
		// 		// Serial.println(F("DB:TIMEOUT"));Serial.write(ACK_BIT);
		// 		timeout = true;
		// 	}
		// }
	} while(processing && !timeout);

	return !timeout;
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

	cmd.type = type;
	cmd.id = readInt();

	if(Command::isDeviceCommand(type)){
		cmd.deviceID = readInt();
		cmd.value = readLong();
	}else if(Command::isSimpleCommand(type)){
		cmd.value = readLong();
		cmd.deviceID = 0;
	}else{
		cmd.deviceID = 0;
		cmd.value = 0;
	}

	#if DEBUG_CON || DEBUG_ON_PC
		Serial.print("DB:CMD:");
		Serial.print(cmd.type);Serial.print(";");
		Serial.print(cmd.id);Serial.print(";");
		Serial.print(cmd.deviceID);Serial.print(";");
		Serial.print(cmd.value);Serial.print("");
		Serial.write(ACK_BIT);
	#endif

	onMessageReceived(cmd);
}

void DeviceConnection::onMessageReceived(Command cmd) {
	notifyListeners(cmd);
}

void DeviceConnection::getBuffer(uint8_t buf[]){

	for(int a = 0;a < current_length();a++){
		buf[a] = _buffer[a];
	}
}

void DeviceConnection::printBuffer(){
	Serial.println((const char *)_buffer);
}


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
	flush();
	write(START_BIT);
}

void DeviceConnection::doToken(){ // FIXME: change name to putToken();
	write(SEPARATOR);
}

void DeviceConnection::doEnd(){ // FIXME: change name to putEnd();
	write(ACK_BIT);
	flush();
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


void DeviceConnection::send(Command cmd, bool complete){
	if(!conn || processing) return;
    // digitalWrite(10, LOW);
	unsigned long values[] = {cmd.type, cmd.id, cmd.deviceID, cmd.value};

	conn->flush();

	conn->write(START_BIT);
	char vbuffer[3];
	for (int i = 0; i < 4; ++i) {
		utoa(values[i], vbuffer, 10);
		conn->write(vbuffer);
		if(complete && i == 3){
			conn->write(ACK_BIT);
		}else{
			conn->write(SEPARATOR);
		}
	}


}


void DeviceConnection::flush() {
  memset(_buffer, 0, _len);
  if(conn) conn->flush();
  _endOffset = 0;
  _readOffset = 0;
  _buffer_overflow = false;
}



////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


size_t DeviceConnection::store(uint8_t byte) {
  _buffer_overflow = _endOffset >= _len;
  _buffer[_endOffset++] = byte;
  return !_buffer_overflow;
}


int DeviceConnection::available(void){

  return (unsigned int)(_len + _endOffset - _readOffset) % _len;

}

int DeviceConnection::peek(void){

  return (_endOffset != _readOffset ? _buffer[_readOffset] : -1);

}

int DeviceConnection::read(void){

	return (_endOffset != _readOffset ? _buffer[_readOffset++] : -1);

}

int DeviceConnection::readLongValues(long values[], int max){
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseInt();
		size++;
	}

	_readOffset+=2;// skip separator '];'
	_endOffset = lastOffset;

	return size;
}

int DeviceConnection::readIntValues(int values[], int max){
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseInt();
		size++;
	}

	_readOffset+=2; // skip separator '];'
	_endOffset = lastOffset;

	return size;
}

int DeviceConnection::readFloatValues(float values[], int max)
{
	int size = 0;
	memset(values, 0, max);
	int lastOffset = _endOffset;
	_endOffset = nextEndOffSet();

	while(available() && size < max){
		values[size] = parseFloat();
		size++;
	}

	_readOffset+=2;// skip separator '];'
	_endOffset = lastOffset;

	return size;
}

String DeviceConnection::readString()
{
	  String ret;
	  int c = read();

	  if(c == Command::SEPARATOR || c == ',') c = read(); // skip first

	  while (c >= 0 && c != Command::SEPARATOR && c != ',')
	  {
	    ret += (char) c;
	    c = read();
	  }
	  return ret;
}

// returns the first valid (long) integer value from the current position.
// initial characters that are not digits (or the minus sign) are skipped
// function is terminated by the first character that is not a digit.
long DeviceConnection::parseInt()
{
  bool isNegative = false;
  long value = 0;
  int c;

  c = peekNextDigit();
  // ignore non numeric leading characters
  if(c < 0)
    return 0; // zero returned if timeout

  do{
    if(c == '-')
      isNegative = true;
    else if(c >= '0' && c <= '9')        // is c a digit?
      value = value * 10 + c - '0';
    read();  // consume the character we got with peek
    c = peek();
  }
  while( (c >= '0' && c <= '9'));

  if(isNegative)
    value = -value;
  return value;
}


// as parseInt but returns a floating point value
float DeviceConnection::parseFloat(){
  bool isNegative = false;
  bool isFraction = false;
  long value = 0;
  char c;
  float fraction = 1.0;

  c = peekNextDigit();
    // ignore non numeric leading characters
  if(c < 0)
    return 0; // zero returned if timeout

  do{
    if(c == '-')
      isNegative = true;
    else if (c == '.')
      isFraction = true;
    else if(c >= '0' && c <= '9')  {      // is c a digit?
      value = value * 10 + c - '0';
      if(isFraction)
         fraction *= 0.1;
    }
    read();  // consume the character we got with peek
    c = peek();
  }
  while( (c >= '0' && c <= '9')  || c == '.');

  if(isNegative)
    value = -value;
  if(isFraction)
    return value * fraction;
  else
    return value;
}


// returns peek of the next digit in the stream or -1 if timeout
// discards non-numeric characters
int DeviceConnection::peekNextDigit()
{
  int c;
  while (1) {
    c = peek();
    if (c < 0) return c;  // timeout
    if (c == '-') return c;
    if (c >= '0' && c <= '9') return c;
    read();  // discard non-numeric
  }
}


int DeviceConnection::nextEndOffSet(){
	int endOffset = _readOffset;
	for(int end = _readOffset;end < _endOffset;end++){
		if (isListEnd(_buffer[end]) && end != _readOffset) break;
		endOffset++;
	}

	return endOffset;
}

bool DeviceConnection::isListEnd(char c){
	return  c==']' || c==')' || c=='}' || c==Command::SEPARATOR;
}
