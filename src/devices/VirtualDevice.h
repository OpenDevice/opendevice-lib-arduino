/*
 * VirtualDevice.h
 *
 *  Created on: 27 de ago de 2018
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_DEVICES_VIRTUALDEVICE_H_
#define LIBRARIES_OPENDEVICE_SRC_DEVICES_VIRTUALDEVICE_H_

#include "Device.h"

class VirtualDevice: public Device {
public:
	VirtualDevice(DeviceType _type = DeviceType::DIGITAL);
	virtual ~VirtualDevice();

	bool setValue(value_t value, bool sync);
	value_t getValue();
	void init();
};

#endif /* LIBRARIES_OPENDEVICE_SRC_DEVICES_VIRTUALDEVICE_H_ */
