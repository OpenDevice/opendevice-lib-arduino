/*
 * TcpConnection.cpp
 *
 *  Created on: 22/06/2014
 *  Author: Ricardo JL Rufino
 */

#include "TcpConnection.h"

TcpConnection::TcpConnection(uint16_t port) : DeviceConnection(){
	_port = port;
	 server = new EthernetServer(_port);
	 client = NULL;
	_connected = false;
}

TcpConnection::~TcpConnection() {
	// TODO Auto-generated destructor stub
}

void TcpConnection::init(){
	// OpenDeviceConnection::init();
}

void TcpConnection::connect(){
	byte mac[6] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
    IPAddress myIP(192, 168, 0, 204);
    Ethernet.begin(mac, myIP);
	server->begin();
}

bool TcpConnection::checkDataAvalible(void) {

	// Serial.println("DB:ENC28J60:checkDataAvalible..");
	//print("DB:ENC28J60:checkDataAvalible");
	//doEnd();

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
