/*
 * IRDevice.h
 *
 *  Created on: 30/01/2015
 *      Author: ricardo
 */

#ifndef IRDEVICE_H_
#define IRDEVICE_H_

#include "DeviceManager.h"
#ifndef DEBUG_ON_PC
#include "../IRremote/IRremote.h"
#endif

class IRDevice : public Device  {
public:
	IRDevice();
	IRDevice(uint8_t iid, uint8_t ipin);
	virtual ~IRDevice();
	virtual void deserializeExtraData(Command *cmd, DeviceConnection *conn);

private:
#ifndef DEBUG_ON_PC
	IRsend irsend;
#endif
};

#endif /* IRDEVICE_H_ */
