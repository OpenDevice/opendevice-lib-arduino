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

/*
 * IRDevice.h
 *
 *  Created on: 30/01/2015
 *      Author: ricardo
 */

#ifndef IRDEVICE_H_
#define IRDEVICE_H_

#include "Device.h"
#ifndef DEBUG_ON_PC
#include "../../IRremote/IRremote.h"
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
