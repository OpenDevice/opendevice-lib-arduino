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
 * EthernetServerConnection.h
 *
 *  Created on: 19/02/2015
 *      Author: Ricardo JL Rufino
 */
#include <Arduino.h>
#include <Stream.h>

#if defined (UIPETHERNET_H)
	#include <UIPEthernet.h>
#else
	#include <Ethernet.h>
#endif

#include "config.h"
#include "DeviceConnection.h"

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS EthernetClient

EthernetServer ethserver(80);
EthernetClient ethclient;
IPAddress myIP(192, 168, 2, 106);

/** This method is called automatically by the OpenDevice when run: ODev.begin() */
void custom_connection_begin(){
	Serial.println(" - Setup EthernetServer");
	byte mac[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };

	//TODO: A configuração se é IP fixo ou DHCP deve ser informada em algum lugar.
//	if (Ethernet.begin(mac) == 0) {
		Serial.println("Failed to configure Ethernet using DHCP");
		Ethernet.begin(mac, myIP);
//	}
	Serial.print("Server is at: "); Serial.println(Ethernet.localIP());
	ethserver.begin();

}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
CUSTOM_CONNECTION_CLASS custom_connection_loop(DeviceConnection *conn){

	if (EthernetClient newClient = ethserver.available()) {
		ethclient = newClient;
	}

	return ethclient;

}

