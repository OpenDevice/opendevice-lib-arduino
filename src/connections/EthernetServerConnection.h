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
 * This implementation is designed to run both the Ethernet Arduino native library when the "arduino_uip" used to work with the ENC28J60 module.<br/>
 * This include will be used automatically when the Ethernet or UIPEthernet library is declared in main sketch
 *
 * Created on: 22/06/2014
 * Update: 23/02/2015
 * Author: Ricardo JL Rufino
 */
#include <Arduino.h>
#include <Stream.h>

#if defined (UIPETHERNET_H)
	#include <UIPEthernet.h>
#else
	#include <Ethernet.h>
#endif

// NOTE: Please do not include OpenDevice.h this will break the preprocessors / macros
#include "config.h"
#include "DeviceConnection.h"

using namespace od;

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS EthernetClient

EthernetServer ethserver(DEFAULT_SERVER_PORT);
EthernetClient ethclient;


/** This method is called automatically by the OpenDevice when run: ODev.begin() */
void custom_connection_begin(){
	// Serial.println(" - Setup EthernetServer");

	// uint8_t *mac = Config.id;
  // FIXME: generate MAC.
	byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

	// if(!Config.saved){
	// 	loadConfig();
	// }

	// TODO: remove logic to save IP, implement discovery service;

	// Using saved IP (Sketch or EEPROM)
	if(Config.ip[0] != 0){
		Serial.println(F("Using saved IP on EEPROM"));
		Ethernet.begin(mac, Config.ip);
	} // try DHCP
	else{
		Serial.println(F("Using DHCP"));
		#if(ENABLE_DHCP)
			#if defined (UIPETHERNET_H) && !UIP_CONF_UDP
			#error "Using UIPEthernet with DHCP, you must enable UIP_CONF_UDP ! (This eats space !)"
			#endif
			if (Ethernet.begin(mac)>0) {
				IPAddress ip = Ethernet.localIP();
				Config.ip[0] = ip[0];
				Config.ip[1] = ip[1];
				Config.ip[2] = ip[2];
				Config.ip[3] = ip[3];
				ODev.save();
			}else{
				Serial.println("DHCP Failed");
			}
		#else
			Serial.println(F("Please define a IP or enable DHCP"));
		#endif
	}
	//TODO: A configuração se é IP fixo ou DHCP deve ser informada em algum lugar.
//	if (Ethernet.begin(mac) == 0) {


//	}
	Serial.print("Server is at: "); Serial.println(Ethernet.localIP());
	ethserver.begin();




}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
EthernetClient custom_connection_loop(DeviceConnection *conn){

	if (EthernetClient newClient = ethserver.available()) {
		ethclient = newClient;
	}

	return ethclient;

}
