/*
 * PulseCounter.h
 *
 *  Created on: 3 de ago de 2018
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_DEVICES_PULSECOUNTER_H_
#define LIBRARIES_OPENDEVICE_SRC_DEVICES_PULSECOUNTER_H_

#include "Device.h"
#include <FunctionalInterrupt.h>

class PulseCounter: public Device {
public:
	PulseCounter(uint8_t pin, uint32_t _debounceTime = 0);
	virtual ~PulseCounter();

	bool setValue(value_t value, bool sync);
	void init();
	bool hasChanged();

	void onInterrupt(void);

	uint32_t count = 0;
	uint32_t pulseLength = 0;

private:
	uint32_t debounceTime;

};

#endif /* LIBRARIES_OPENDEVICE_SRC_DEVICES_PULSECOUNTER_H_ */
