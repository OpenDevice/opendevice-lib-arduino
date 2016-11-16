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
 * Wifi/Ethernet connection for Arduino YUN
 * Created on: 12/01/2016
 * Author: Ricardo JL Rufino
 */

#include <Bridge.h>
#include <YunClient.h>
#include <YunServer.h>

class DeviceConnection;

using namespace od;

#define USING_CUSTOM_CONNECTION 1
#define CUSTOM_CONNECTION_CLASS YunClient

YunServer ethserver;
YunClient ethclient;


/** This method is called automatically by the OpenDevice when run: ODev.begin() */
void custom_connection_begin(){
	Bridge.begin();
//	ethserver.listenOnLocalhost();
	ethserver.begin();
}

/** This method is called automatically by the OpenDevice when run: ODev.loop() */
YunClient custom_connection_loop(DeviceConnection *conn){

	if (YunClient newClient = ethserver.accept()) {
		ethclient = newClient;
	}

	return ethclient;

}
