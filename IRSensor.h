/*
 * IRSensor.h
 *
 *  Created on: 29/04/2013
 *  Author: Ricardo JL Rufino
 */

#ifndef ENERGIA // Bypass on Stellaris

#ifndef IRSENSOR_H_
#define IRSENSOR_H_

#include "Stream.h"
#include "DeviceManager.h"
#include "DataUtils.h"
#include "../IRremote/IRremote.h"

class IRSensor : public Device {
public:
	IRSensor();
	IRSensor(uint8_t iid, uint8_t ipin);
	virtual void init();
	virtual bool hasChanged();
	virtual size_t serializeExtraData(DeviceConnection *conn);
	virtual ~IRSensor();

private:
	IRrecv irrecv;
	decode_results results;

};

#endif /* IRSENSOR_H_ */

#endif
