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

#include "IRDevice.h"

/*
 * IRDevice.cpp
 *
 *  Created on: 30/01/2015
 *      Author: ricardo
 */
IRDevice::IRDevice() {
}

IRDevice::IRDevice(uint8_t iid, uint8_t ipin) :
	Device(iid, ipin, Device::INFRA_RED, false){
}

IRDevice::~IRDevice() {
}

void IRDevice::deserializeExtraData(Command *cmd, DeviceConnection *conn){

	#ifdef DEBUG_ON_PC
		bool debug = true;
	#else
		bool debug = false;
	#endif

	int protocol = conn->readInt();
	/*debug*/if(debug) printf("protocol = %d \n", protocol);
	int labels[10]= {0};
	int length = conn->readIntValues(labels,10);
	/*debug*/if(debug) printf("Labels(%d) : " , length);
	for (int i = 0; i < length; ++i) {
		if(debug) printf("%d,", labels[i]);
	}
	if(debug) printf("\n");

//	char *values;
//	length = conn->readString(&values);
	String values = conn->readString();
	if(debug) printf("Values (%d) : %s\n", length, values.c_str());

	unsigned int rawvalues[length];

//	Serial.print("DB:");

	for (int i = 0; i < length; ++i) {
		int val = values[i];
		int label = (val - 65); // convert to int
		rawvalues[i] = labels[label];
		if(debug) printf("%d,", rawvalues[i]);
//		Serial.print(rawvalues[i], 10);
//		Serial.print(',');
	}

	if(debug) printf("\n");

//	Serial.write(ACK_BIT);

#ifndef DEBUG_ON_PC
	irsend.sendRaw(rawvalues,length,32);
#endif

}
