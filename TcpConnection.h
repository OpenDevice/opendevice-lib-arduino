/*
 * TcpConnection.h
 *
 *  Created on: 16/11/2014
 *      Author: ricardo
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <Arduino.h>
#include "EthernetIncludes/Ethernet.h"
#include "DeviceConnection.h"

class TcpConnection : public DeviceConnection {
private:
	uint16_t _port;
	virtual void init();
	bool _connected;
	void connect();
public:
	TcpConnection(uint16_t);
	virtual ~TcpConnection();
	virtual bool checkDataAvalible(void);

	EthernetServer *server; // TODO : remover de publico...
	EthernetClient *client;
};

#endif /* TCPCONNECTION_H_ */
