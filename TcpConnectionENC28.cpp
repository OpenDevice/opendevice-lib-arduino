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
 * TcpConnectionENC28.cpp
 *
 *  Created on: 22/06/2014
 *      Author: ricardo
 */

#include "TcpConnectionENC28.h"

TcpConnectionENC28::TcpConnectionENC28(uint16_t port) : DeviceConnection(){
	_port = port;
	 server = new EthernetServer(_port);
	 client = NULL;
	_connected = false;
}

TcpConnectionENC28::~TcpConnectionENC28() {
	// TODO Auto-generated destructor stub
}

void TcpConnectionENC28::init(){
	// OpenDeviceConnection::init();
}

void TcpConnectionENC28::connect(){
	uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress myIP(192, 168, 0, 204);

    // Ethernet.setSSPIN(5, 5); // Default is 10;

    Serial.println("DB:begin");
    Ethernet.begin(mac, myIP);
    Serial.println("DB:begin[ok]");
	server->begin();

	Serial.println("DB:Connect [ok]");
}

bool TcpConnectionENC28::checkDataAvalible(void) {

	//Serial.println("DB:ENC28J60:checkDataAvalible..");
	//print("DB:ENC28J60:checkDataAvalible");
	//doEnd();
	//delay(500);

	if(!_connected){
		connect();
		delay(1000);
		_connected = true;
	}


	if (EthernetClient newClient = server->available()) {

		// only if new Client.
		if(!client || client != &newClient){

			client = &newClient;

			Serial.println("DB:CONNECTED!");
		}

	}

	if (client && client->connected()) {

		com = (Stream*) client;
		DeviceConnection::checkDataAvalible(); // Call supper class

	}
}
