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

#if defined(_ADAFRUIT_SENSOR_H)

#include "AdafruitSensor.h"

AdafruitSensor::AdafruitSensor(Adafruit_Sensor* _impl, DeviceType type): Device(0, 0, type, true),
	impl(_impl) {
}

#if defined(DHT_U_H)
AdafruitSensor::AdafruitSensor(DHT_Unified::Temperature& _sensor): Device(0, 0, Device::FLOAT2_SIGNED, true),
	impl(&_sensor) {
}

AdafruitSensor::AdafruitSensor(DHT_Unified::Humidity& _sensor): Device(0, 0, Device::FLOAT2_SIGNED, true),
	impl(&_sensor) {
}
#endif

AdafruitSensor::~AdafruitSensor() {

}

void AdafruitSensor::init(){
	sensor_t sensor;
	impl->getSensor(&sensor);
	usType = sensor.type;
}

bool AdafruitSensor::updateValue(float val){

		// TODO !!!  ////////////////////// CASTTTTTTTTTTTTTTTTTTTTTTT || NEGATIVOOOOO
		value_t cVal = val;

	 if(!isnan(val) && currentValue != cVal){
		 currentValue = cVal;
		 return true;
	 }else{
		 return false;
	 }
}

bool AdafruitSensor::hasChanged(){

	sensors_event_t event;
  impl->getEvent(&event);

	switch (usType) {
		case SENSOR_TYPE_AMBIENT_TEMPERATURE:
				// Serial.print("Temperature: ");
				// Serial.print(event.temperature);
				// Serial.println(" *C");
			return updateValue(event.temperature);
		case SENSOR_TYPE_RELATIVE_HUMIDITY:
				// Serial.print("Humidity: ");
				// Serial.print(event.relative_humidity);
				// Serial.println("%");
			return updateValue(event.relative_humidity);
	}
	return false;

}

#endif
