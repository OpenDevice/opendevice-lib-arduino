/*
 * Timeout.h
 *
 *  Created on: 14/11/2015
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_UTILITY_TIMEOUT_H_
#define LIBRARIES_OPENDEVICE_SRC_UTILITY_TIMEOUT_H_


#include <Arduino.h>

namespace od {

class Timeout {
public:
	Timeout(uint16_t timeout, bool enabled =  false);
	virtual ~Timeout();
	bool expired();
	void reset();
	void enable();
	void disable();
	bool isEnabled() { return enabled;}
private:
	uint16_t _timeout;
	bool enabled;
	unsigned long lastCheck;
};

} /* namespace od */

#endif /* LIBRARIES_OPENDEVICE_SRC_UTILITY_TIMEOUT_H_ */
