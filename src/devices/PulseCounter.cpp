/*
 * PulseCounter.cpp
 *
 *  Created on: 3 de ago de 2018
 *      Author: ricardo
 */

#include "PulseCounter.h"

PulseCounter::PulseCounter(uint8_t ipin, uint32_t _debounceTime) :
   Device(0, ipin, Device::ANALOG, true) {
	debounceTime = _debounceTime;

}

PulseCounter::~PulseCounter() {

}

void PulseCounter::onInterrupt(void)
{
	unsigned long diff = millis() - readLastTime;
	if(readLastTime == 0 || diff > debounceTime){

		pulseLength = diff;
		count++;

//		double impkWh = 1600;
//		double pulseSec = (diff) / 1000.0;
//		double power = ((1/impkWh)/pulseSec) * (3600 * 1000);

		readLastTime = millis();

//		Serial.print("Count:");
//		Serial.println(count);
//		Serial.print("Pulse(ms):");
//		Serial.println(diff);
//		Serial.print("Power:");
//		Serial.println(power);

	}
}

void PulseCounter::init(){
	pinMode(pin, INPUT_PULLUP);
	attachInterrupt(pin, std::bind(&PulseCounter::onInterrupt, this), RISING);
}

bool PulseCounter::hasChanged(){

	bool changed = count != currentValue;

	currentValue = count;

	return changed;
}


bool PulseCounter::setValue(value_t value, bool sync){
	currentValue = value;
	count = value; // restore previous value
	return true;
}

