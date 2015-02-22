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
 *      Author: ricardo
 */
#include <Arduino.h>
#include "config.h"

#if defined (UIPETHERNET_H)
	#include <UIPEthernet.h>
#else
	#include <Ethernet.h>
#endif

#include <Stream.h>
#include "DeviceConnection.h"

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS EthernetClient

EthernetServer server(80);
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
	server.begin();

}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
CUSTOM_CONNECTION_CLASS custom_connection_loop(DeviceConnection *conn){

	if (EthernetClient newClient = server.available()) {

		ethclient = newClient;

//		// only if new client.
//		if (!client || client != &newClient) {
//
//			client = &newClient;
//		}

//		if (newClient.connected()) {

			Serial.println("Connected"); delay(200);

//			conn->setStream((Stream*) &newClient);
//			conn->checkDataAvalible();
//		}

	}else{
//		conn->setStream(NULL);
	}

	return ethclient;

}
//
//void ext_connection_readdata(EthernetClient& p_client){
//	if(client){
//		Serial.println("DB:ext_connection_readdata!"); delay(500);
//		    boolean currentLineIsBlank = true;
//		    while (client.connected()) {
//		    	Serial.println("DB:ext_connection_conected !"); delay(500);
//
//		      if (client.available()) {
//		        char c = client.read();
//		        Serial.write(c);
//		        // if you've gotten to the end of the line (received a newline
//		        // character) and the line is blank, the http request has ended,
//		        // so you can send a reply
//		        if (c == '\n' && currentLineIsBlank) {
//		          // send a standard http response header
//		          client.println("HTTP/1.1 200 OK");
//		          client.println("Content-Type: text/html");
//		          client.println("Connection: close");  // the connection will be closed after completion of the response
//		          client.println();
//		          client.println("<!DOCTYPE HTML>");
//		          client.println("<html>");
//		          // output the value of each analog input pin
//		          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//		            int sensorReading = analogRead(analogChannel);
//		            client.print("analog input ");
//		            client.print(analogChannel);
//		            client.print(" is ");
//		            client.print(sensorReading);
//		            client.println("<br />");
//		          }
//		          client.println("</html>");
//		          break;
//		        }
//		        if (c == '\n') {
//		          // you're starting a new line
//		          currentLineIsBlank = true;
//		        }
//		        else if (c != '\r') {
//		          // you've gotten a character on the current line
//		          currentLineIsBlank = false;
//		        }
//		      }
//		    }
//
//		    // give the web browser time to receive the data
//			delay(1);
//			// close the connection:
//			client.stop();
//	}
//}


