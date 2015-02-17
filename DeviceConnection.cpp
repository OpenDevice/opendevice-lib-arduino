
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

DeviceConnection::DeviceConnection(Stream &stream) {
	com = &stream;
	init();
}


void DeviceConnection::init(){

	numberOfValues = 0;
	waitTime = 30;
	bufferOffset = 0;
	bufferCount = 0;

	defaultListener = NULL;
	for(int a = 0;a < MAX_LISTENERS;a++){
		listeners[a] = NULL;
		listeners_key[a] = NULL;
	}
}


void DeviceConnection::addListener(uint8_t commandType, CommandListener listener ){
	if (commandType >= 0 && commandType < MAX_LISTENERS){
		listeners[listenersLength] = listener;
		listeners_key[listenersLength] = commandType;
		listenersLength++;
	}
}

void DeviceConnection::setDefaultListener(CommandListener listener ){
	defaultListener = listener;
}

void DeviceConnection::removeListener(uint8_t command){

	// Only remove pointers
	for (uint8_t i = 0; i < listenersLength; ++i) {
		if(listeners_key[i] == command){
			listeners_key[i] = NULL;
			listeners[i] = NULL;
			break;
		}
	}

}

bool DeviceConnection::checkDataAvalible(){
	uint8_t lastByte;
	bool timeout = false;

	while(!timeout)
	{

		#if DEBUG_CON
			delay(500);
			Serial.print("DB:available=");
			Serial.println(com->available());
			Serial.write(19);
		#endif

		while(com->available() > 0)
		{

			lastByte = com->read();

			#if DEBUG_CON
				Serial.print("DB:READ:");
				Serial.println(lastByte);
				Serial.write(19);
			#endif

			if(lastByte == STOP_BIT){
				flush();
			}
			else if(lastByte == ACK_BIT){
				parseCommand();
				flush();
			}
			else if(bufferCount < DATA_BUFFER){
				buffer[bufferCount] = lastByte;
				bufferCount++;
			}
			else{
				notifyError(ResponseStatus::BUFFER_OVERFLOW);
				#if DEBUG_CON
				Serial.print("DB:BUFFER_OVERFLOW");Serial.write(19);
				#endif
				return false;
			}
		}
		
		if(com->available() <= 0 && !timeout){
			if(waitTime > 0) delayMicroseconds(waitTime);
			if(com->available() <= 0) timeout = true;
		}
	}
	return timeout;
}


// Private methods
void DeviceConnection::notifyListeners(Command cmd){

	if(defaultListener) (*defaultListener)(cmd);
	// Notify specific listener
	for (uint8_t i = 0; i < listenersLength; ++i) {
		if(listeners_key[i] == cmd.type){ // confirm type
			(*listeners[i])(cmd);
			break;
		}
	}

}


void DeviceConnection::notifyError(ResponseStatus::ResponseStatus status){
	Command cmd;
	cmd.type = CommandType::DEVICE_COMMAND_RESPONSE;
	cmd.deviceID = 0;
	cmd.id = 0;
	cmd.value = status;
	send(cmd, true);
}

void DeviceConnection::parseCommand(){

	Command cmd;
	int data[4] = {}; readIntValues(data, 4);
	cmd.type = (uint8_t) data[0];
	cmd.id = (uint8_t) data[1];
	cmd.deviceID = (uint8_t) data[2];
	cmd.value = (unsigned long) data[3]; // TODO: Observar conversão de INT -> LONG

	#if DEBUG_CON || DEBUG_ON_PC
		Serial.print("DB:CMD:");
		Serial.print(cmd.type);Serial.print(";");
		Serial.print(cmd.id);Serial.print(";");
		Serial.print(cmd.deviceID);Serial.print(";");
		Serial.print(cmd.value);Serial.print("");
		Serial.write(19);
	#endif


	notifyListeners(cmd);

	memset(buffer, 0, sizeof buffer);
	bufferOffset = 0;
	bufferCount = 0;

}

void DeviceConnection::getBuffer(uint8_t buf[]){

	for(int a = 0;a < bufferCount;a++){
		buf[a] = buffer[a];
	}
}

int DeviceConnection::readString(char** target){
	int length = nextEndOffSet() - bufferOffset;
	*target = (char*) buffer;
	*target += bufferOffset;
	buffer[bufferOffset+length] = '\0'; // end string, replace ';'
	bufferOffset = length + 1;
	return length;
}

//int DeviceConnection::readString(char target[]){
//
//	int endOffset = nextEndOffSet();
//	int pos = 0;
//	for(int a = bufferOffset;a < endOffset;a++){
//		if(isSeparator(buffer[a])) break;
//		target[pos++] = buffer[a];
//	}
//	target[pos] = '\0';
//	bufferOffset += pos+1;
//	return pos;
//}

int DeviceConnection::readInt(uint8_t endOffset){

	// find endOffset ( ';' or ',' or ']' ... )
	if(endOffset == 0){
		endOffset = nextEndOffSet();
	}

	uint8_t len = endOffset-bufferOffset;
	uint8_t b[len+1]; // +1 for '\0';

	int pos = 0;
	for(int a = bufferOffset; a < endOffset;a++){
		b[pos++] = buffer[a];
		// printf("(%c)",buffer[a]);
	}

	b[len] = '\0';
	bufferOffset += len;
	bufferOffset++; // skip ';'
	return atoi((char*)b);
}

// FIX to new !!
long DeviceConnection::readLong()
{
	uint8_t b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atol((char*)b);
}

float DeviceConnection::readFloat()
{
	return (float)readDouble();
}

int DeviceConnection::getArrayLength()
{
	if (bufferCount == 1) return 0; // only a flag and ACK_BIT was sent, not data attached
	numberOfValues = 1;
	// find the amount of values we got
	for (int a=1; a<bufferCount;a++){
		if (buffer[a]==TOKEN_BIT) numberOfValues++;
	}
	return numberOfValues;
}

void DeviceConnection::readFloatValues(float values[])
{
	int t = 0; // counter for each char based array
	int pos = 0;

	int start = 1; // start of first value
	for (int end=1; end<bufferCount;end++){
		// find end of value
		if (buffer[end]==TOKEN_BIT) {
			// now we know start and end of a value
			char b[(end-start)+1]; // create container for one value plus '\0'
			t = 0;
			for(int i = start;i < end;i++){
				b[t++] = (char)buffer[i];
			}
			b[t] = '\0';
			values[pos++] = atof(b);
			start = end+1;
		}
	}
	// get the last value
	char b[(bufferCount-start)+1]; // create container for one value plus '\0'
	t = 0;
	for(int i = start;i < bufferCount;i++){
		b[t++] = (char)buffer[i];
	}
	b[t] = '\0';
	values[pos] = atof(b);
}

// not tested yet
void DeviceConnection::readDoubleValues(float values[])
{
	readFloatValues(values);
}


/**
 * Can read single value list: 1;2;3;4 or array like: [1,2,3,4]
 * If you need to read two different arrays like: [1,2,3];[5,2,3,4] call the method 'readIntValues' twice
 */
int DeviceConnection::readIntValues(int values[], int max)
{
	int pos = 0;

	for (int end=bufferOffset; end<bufferCount;end++){

		// If is a start char '[ , (' skip to next
		if(isListStart(buffer[end])){
			bufferOffset++;
			continue;
		}

		bool listEnd = isListEnd(buffer[end]) || (end+1 == bufferCount);

		// find end of value
		if (isSeparator(buffer[end]) || listEnd) {

			if(end+1 == bufferCount) end++; // read last value

			values[pos++] = readInt(end);
			bufferOffset = end+1; // skip next separator

			if((max > 0 && pos == max) || listEnd){
				if(listEnd) bufferOffset++; // skip next separator '];'
				break;
			}

		}

	}

	if(bufferOffset > bufferCount) bufferOffset = bufferCount;
	//printf("size : %d", pos);
	return pos;
}


double DeviceConnection::readDouble()
{
	char b[bufferCount];
	for(int a = 1;a < bufferCount;a++){
		b[a-1] = (char)buffer[a];
	}

	b[bufferCount-1] = '\0';
	return atof(b);
	
}


#if defined(ARDUINO) && ARDUINO >= 100
size_t DeviceConnection::write(uint8_t b){
	return com->write(b);
}
#else
void DeviceConnection::write(uint8_t b){
	com->write(b);
}
#endif
	


void DeviceConnection::doStart(){
	com->write(START_BIT);
}

void DeviceConnection::doToken(){
	com->write(TOKEN_BIT);
}

void DeviceConnection::doEnd(){
	com->write(ACK_BIT);
}


void DeviceConnection::send(char c ){
	com->write(START_BIT);
	com->write(c);
	com->write(ACK_BIT);
}

void DeviceConnection::send(const char str[]){
	com->write(START_BIT);
	com->write(str);
	com->write(ACK_BIT);
}

void DeviceConnection::send(long values[], int size){
	com->write(START_BIT);
	char vbuffer[3];
	for (int i = 0; i < size; ++i) {
		ltoa(values[i], vbuffer, 10);
		com->write(vbuffer);
		com->write(TOKEN_BIT);
	}
	com->write(ACK_BIT);
}


void DeviceConnection::send(int values[], int size){
	com->write(START_BIT);
	char vbuffer[3]; // 3 digits
	for (int i = 0; i < size; ++i) {
		itoa(values[i], vbuffer, 10);
		com->write(vbuffer);
		com->write(TOKEN_BIT);
	}
	com->write(ACK_BIT);
}

void DeviceConnection::send(uint8_t n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::send(int n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::send(unsigned int n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::send(long n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::send(unsigned long n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::send(long n, int base){
	com->write(START_BIT);
	com->print(n, base);
	com->write(ACK_BIT);
}
void DeviceConnection::send(double n){
	com->write(START_BIT);
	com->write(n);
	com->write(ACK_BIT);
}
void DeviceConnection::sendln(void){
	com->write(START_BIT);
	com->println();
	com->write(ACK_BIT);
}


void DeviceConnection::send(Command cmd, bool complete){
	com->write(START_BIT);
	com->print(cmd.type);
	com->write(TOKEN_BIT);
	com->print(cmd.id);
	com->write(TOKEN_BIT);
	com->print(cmd.deviceID);
	com->write(TOKEN_BIT);
	com->print(cmd.value);
	if(complete)
		com->write(ACK_BIT);
	else
		com->write(TOKEN_BIT);
}

int DeviceConnection::nextEndOffSet(){
	int endOffset = bufferOffset;
	for(int end = bufferOffset;end < bufferCount;end++){
		if (isSeparator(buffer[end])) break;
		endOffset++;
	}

	return endOffset;
}

bool DeviceConnection::isListEnd(char c){
	return  c==']' || c==')' || c=='}';
}

bool DeviceConnection::isSeparator(char c){
	return c==TOKEN_BIT || c==',';
}

bool DeviceConnection::isListStart(char c){
	return c=='[' || c=='(' || c=='{';
}

void DeviceConnection::flush(){
	for(int a = 0; a < DATA_BUFFER; a++){
		buffer[a] = 0;
	}
	bufferCount = 0;
	numberOfValues = 0;
}
