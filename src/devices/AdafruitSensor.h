/*
 * ******************************************************************************
 *  Copyright (c) 2013-2014 CriativaSoft (www.criativasoft.com.br)
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *  Ricardo JL Rufino - Initial API and Implementation
 * *****************************************************************************
 */

#ifndef AdafruitSensor_H_
#define AdafruitSensor_H_

#include "Device.h"
#include <Adafruit_Sensor.h>

#if defined(DHT_U_H)
#include <DHT.h>
#include <DHT_U.h>
#endif

/**
 * Adapter for Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
 */
class AdafruitSensor : public Device  {
public:
	AdafruitSensor(Adafruit_Sensor* _impl, DeviceType type);

	#if defined(DHT_U_H)
	AdafruitSensor(DHT_Unified::Temperature& _sensor);
	AdafruitSensor(DHT_Unified::Humidity& _sensor);
	#endif


	virtual ~AdafruitSensor();
	void init();
	bool hasChanged();

private:
	Adafruit_Sensor* impl;
	int32_t  usType; /** Sensor types in Adafruit_Sensor.h */

	bool updateValue(float val);
};

#endif /* AdafruitSensor_H_ */
