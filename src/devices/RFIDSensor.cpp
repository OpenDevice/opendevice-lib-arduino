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

#if defined(MFRC522_h)

#include "RFIDSensor.h"

RFIDSensor::RFIDSensor(byte chipSelectPin, byte resetPin) :
		Device(0, 0, Device::NUMERIC, true), mfrc522(chipSelectPin, resetPin) {
}

RFIDSensor::~RFIDSensor() {

}

void RFIDSensor::init(){
	SPI.begin();
	mfrc522.PCD_Init();
}

bool RFIDSensor::hasChanged(){
	// Look for new cards
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return false;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return false;
	}

	if (mfrc522.uid.size == 4) {

		byte *raw = mfrc522.uid.uidByte;

		currentValue = (unsigned long) (raw[3] << 24) | (raw[2] << 16) | (raw[1] << 8) | raw[0];
		mfrc522.PICC_HaltA(); // mark as 'READ'
		return true;
	}

	return false;

}

#endif
