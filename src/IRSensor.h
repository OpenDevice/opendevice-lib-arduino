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


#ifndef ENERGIA // Bypass on Stellaris

#ifndef IRSENSOR_H_
#define IRSENSOR_H_

#include "Stream.h"
#include "Device.h"
#include "DataUtils.h"
#include "../../IRremote/IRremote.h"

/*
 * IRSensor.h
 *
 *  Created on: 29/04/2013
 *  Author: Ricardo JL Rufino
 */
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
