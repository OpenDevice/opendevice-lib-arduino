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
#ifndef LIBRARIES_OPENDEVICE_SRC_CustomSensor_H_
#define LIBRARIES_OPENDEVICE_SRC_CustomSensor_H_

#include "Device.h"

extern "C"
{
  // Definition of the listener function
  typedef value_t (*CommandFunction) ();
}

class CustomSensor : public Device  {
public:
	CustomSensor(CommandFunction func);
	virtual ~CustomSensor();
	bool hasChanged();
private:
	CommandFunction commandFunction;
};

#endif /* LIBRARIES_OPENDEVICE_SRC_CustomSensor_H_ */
