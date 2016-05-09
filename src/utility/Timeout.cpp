/*
 * Timeout.cpp
 *
 *  Created on: 14/11/2015
 *      Author: ricardo
 */

#include "Timeout.h"

namespace od {

Timeout::Timeout(uint16_t time, bool enabled) : _timeout(time), lastCheck(0), enabled(enabled) {

}

Timeout::~Timeout() {
}

void Timeout::reset(){
	lastCheck = millis();
}

void Timeout::enable() {
	enabled = true;
	reset();
}

void Timeout::disable() {
	enabled = false;
}

bool Timeout::expired(){

	if(!enabled) return false;

	unsigned long currentMillis = millis();

	if(lastCheck == 0){
		lastCheck = currentMillis;
		return false;
	}

	if(currentMillis - lastCheck > _timeout){
		lastCheck = currentMillis;
		return true;
	}

	return false;
}

} /* namespace od */
