/*
 * DuplicatedValueFilter.cpp
 *
 *  Created on: 30 de jul de 2018
 *      Author: ricardo
 */

#include "DuplicatedValueFilter.h"


DuplicatedValueFilter::DuplicatedValueFilter(unsigned long interval) : filterInterval(interval) {

}

DuplicatedValueFilter::~DuplicatedValueFilter() {
}


bool DuplicatedValueFilter::accept(value_t value){

	if(value == device->currentValue && filterInterval > 0 && (millis() - device->readLastTime) < filterInterval ){
		//Serial.print("DUPLICATED: ");
		//Serial.print(value);
		//Serial.print(", time: ");
		//Serial.println((millis() - readLastTime));
		return false;
	}

	return true;

}

