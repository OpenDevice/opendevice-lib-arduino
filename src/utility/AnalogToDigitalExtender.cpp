/*
 * ResistiveAnalogInExtender.cpp
 *
 *  Created on: 31 de jul de 2018
 *      Author: ricardo
 */

#include "AnalogToDigitalExtender.h"

AnalogToDigitalExtender::AnalogToDigitalExtender(uint8_t _analogPin , uint8_t _allowedRange)
		: analogPin(_analogPin),
		  allowedRange(_allowedRange),
		  value(1024),
		  time(0){

}

AnalogToDigitalExtender::~AnalogToDigitalExtender() {
}

int AnalogToDigitalExtender::digitalReadEx(uint16_t targetResistence){

//	if(value != 1024){
//		bool c = (targetResistence - allowedRange) < value && value < (targetResistence + allowedRange);
//		Serial.print(targetResistence);Serial.print("->");Serial.print(value);Serial.print( "="); Serial.println(c);
//	}

	if((targetResistence - allowedRange) < value && value < (targetResistence + allowedRange)){
		return HIGH;
	}else{
		return LOW;
	}

}

void AnalogToDigitalExtender::loop(bool debug){

	if(time == 0) time = millis();

	// Debounce readings
	if(millis() - time > 100){
		int read = analogRead(analogPin);
		time = millis(); // reset

		if(debug && value != read){
			Serial.print("Read:");
			Serial.println(read);
		}

		value = read;
	}

}
