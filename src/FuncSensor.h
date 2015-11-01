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
#ifndef LIBRARIES_OPENDEVICE_SRC_FUNCSENSOR_H_
#define LIBRARIES_OPENDEVICE_SRC_FUNCSENSOR_H_

#include "Device.h"

extern "C"
{
  // Definition of the listener function
  typedef unsigned long (*CommandFunction) ();
}

class FuncSensor : public Device  {
public:
	FuncSensor(CommandFunction func);
	virtual ~FuncSensor();
	bool hasChanged();
private:
	CommandFunction fgetValue;
};

#endif /* LIBRARIES_OPENDEVICE_SRC_FUNCSENSOR_H_ */
