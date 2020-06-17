/*
 * PulseCounter.cpp
 *
 *  Created on: 3 de ago de 2018
 *      Author: ricardo
 */

#ifdef FUNCTIONALINTERRUPT_H

#include "PulseCounter.h"

PulseCounter::PulseCounter(uint8_t ipin, int _mode, uint32_t _debounceTime) :
   Device(0, ipin, Device::ANALOG, true) {
	debounceTime = _debounceTime;
	mode = _mode;

}

PulseCounter::~PulseCounter() {

}

void PulseCounter::onInterrupt(void)
{
	unsigned long diff = millis() - readLastTime;
	if(readLastTime == 0 || diff > debounceTime){
		pulseLength = diff;
		count++;
		readLastTime = millis();
	}
}

void PulseCounter::init(){
	attachInterrupt(pin, std::bind(&PulseCounter::onInterrupt, this), mode);
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
#endif

