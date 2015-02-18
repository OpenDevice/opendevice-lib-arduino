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

#include "IRSensor.h"

/*
 * IRSensor.cpp
 *
 *  Created on: 29/04/2013
 *  Author: Ricardo JL Rufino
 */
IRSensor::IRSensor(uint8_t iid, uint8_t ipin) :
	irrecv(IRrecv(ipin)),
	Device(iid, ipin, Device::INFRA_RED, true){
}


IRSensor::~IRSensor() {
}

void IRSensor::init(){
	irrecv.enableIRIn();
}


bool IRSensor::hasChanged(){
	if (irrecv.decode(&results)){
		currentValue = results.value;
		irrecv.resume();
		return true;
	}

	return false;
}

size_t IRSensor::serializeExtraData(DeviceConnection *conn){

	// Return only protocol type
//	if(results.decode_type > 0){
//		itoa(results.decode_type,(char*)buffer, 10);
//		return 1;
//	}

	// Return raw data (processing to reduce the quantity of data)

	int raw_length = results.rawlen;

	uint8_t maxlabel = 10;
    uint8_t labelIndex = 0;
    uint8_t start = 1; // Ignore first (usually always have this space/gap

    unsigned int labels[maxlabel]; // store sequence of labels values;
	unsigned int *rawbuf = (unsigned int *) results.rawbuf;
	unsigned int errorMargin = 250;

	// memset(labels, 0, maxlabel);

    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
	for (int i = 1; i < raw_length; i++) {
		if (i % 2) // Space
			rawbuf[i] = rawbuf[i]*USECPERTICK + MARK_EXCESS;
		else // Mark
			rawbuf[i] = rawbuf[i]*USECPERTICK - MARK_EXCESS;
	}

    // Processes equals/equivalent numbers and map into labels(0...9)
	for (int i = start; i < raw_length; i++) {
    	unsigned int val = rawbuf[i];
        if(i == start){
            labels[labelIndex] = val;
            rawbuf[i] = labelIndex;
        } else if(rawbuf[i] <= maxlabel){ // if has processed
            continue;
        }else{
            labelIndex++;
            labels[labelIndex] = val;
            rawbuf[i] = labelIndex;
        }

        // Abort !
        if(labelIndex == maxlabel) break;

        // find in array and replace to current label,
        for (int j = i+1; j < raw_length; j++) {
        	unsigned int nextval = rawbuf[j];
        	int diff = val - nextval;
            if(abs(diff) <= errorMargin){
                rawbuf[j] = labelIndex;
            }
        }
    }

	conn->print('0'); // UNKNOWN Protocol
	conn->print((char)Command::SEPARATOR);

	// Add Labels/Mappings
	conn->print('[');
	for (int i = 0; i <= labelIndex; i++) {
		conn->print(labels[i]);
		if(i != labelIndex )
			conn->print((char)Command::ARRAY_SEPARATOR);
	}
	conn->print(']');
	conn->print((char)Command::SEPARATOR);

//	// this join all data into single string
//	// At this point the variable 'rawbuf' contains only labels 0..9
//	for (int i = start; i < raw_length; i++) {
//		conn->print((char)(rawbuf[i] + 65));
//	}

//		// this join all data into single string
//		// At this point the variable 'rawbuf' contains only labels 0..9
//		for (int i = start; i < raw_length; i++) {
//			conn->print((char)(rawbuf[i] + '1'));
//		}

	for (int i = start; i < raw_length; i++) {
		rawbuf[i] = rawbuf[i] + 1;
	}

	// this join all data into single string
	// Store two values inside one byte(char)
	// At this point the variable 'rawbuf' contains only labels 0..9
	for (int i = start; i < raw_length; i=i+2) {
        uint8_t v1 =  rawbuf[i];
        uint8_t v2 = ((i + 1 < raw_length) ? rawbuf[i+1] : 0);
        uint8_t res = (char) ((v1 << 4) | (v2 & 0xf));
		conn->write(res);
	}

	return 1; // TODO: RETURN Corrent count !!
}


#endif
