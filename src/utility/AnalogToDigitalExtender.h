/*
 * ResistiveAnalogInExtender.h
 *
 *  Created on: 31 de jul de 2018
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_DEVICES_RESISTIVEANALOGINEXTENDER_H_
#define LIBRARIES_OPENDEVICE_SRC_DEVICES_RESISTIVEANALOGINEXTENDER_H_

#include "Device.h"

/**
 * Transform single analog pin into "multiple digital pins", using resistors.
 * @See: https://opendevice.atlassian.net/wiki/spaces/DOC/pages/412221441/Multiple+Buttons+on+1+Analog
 */
class AnalogToDigitalExtender: public IOExtender {
public:
	AnalogToDigitalExtender(uint8_t _analogPin, uint8_t _allowedRange);
	virtual ~AnalogToDigitalExtender();

	int digitalReadEx(uint16_t pin);
	void loop(bool debug = false);

private:
	uint8_t analogPin;
	uint8_t allowedRange;
	int value;
	unsigned long time;
//	unsigned long timeLoop;
};

#endif /* LIBRARIES_OPENDEVICE_SRC_DEVICES_RESISTIVEANALOGINEXTENDER_H_ */
