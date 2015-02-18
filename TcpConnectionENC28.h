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
 * TcpConnectionENC28.h
 *
 *  Created on: 22/06/2014
 *  Author: Ricardo JL Rufino
 */

#ifndef SOCKETCONNECTIONENC28J60_H_
#define SOCKETCONNECTIONENC28J60_H_

// NOTA: DESABILITADO UDP em UIPEthernet/utility/uipethernet-conf.h:9:#define UIP_CONF_UDP

#include "DeviceConnection.h"
#include "../UIPEthernet/UIPEthernet.h"
#include <Stream.h>

class TcpConnectionENC28 : public DeviceConnection {
private:
	uint16_t _port;
	virtual void init();
	bool _connected;
	void connect();
public:
	TcpConnectionENC28(uint16_t);
	virtual ~TcpConnectionENC28();
	virtual bool checkDataAvalible(void);

	EthernetServer *server; // TODO : remover de publico...
	EthernetClient *client;
};

#endif /* SOCKETCONNECTIONENC28J60_H_ */
