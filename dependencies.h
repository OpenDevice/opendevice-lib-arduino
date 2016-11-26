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


#ifndef LIBRARIES_OPENDEVICE_DEPENDENCIES_H_
#define LIBRARIES_OPENDEVICE_DEPENDENCIES_H_

// *******************************************************************************************
// Configure dependencies used in your project (Please comment unused dependencies)
// *******************************************************************************************


// ===========================================================
// Automatic Detection of Connections
// ===========================================================

#ifdef ESP8266
	#include <ESP8266WiFi.h>
#endif


// #if defined(ethernet_h) || defined (UIPETHERNET_H)
//   #ifdef PubSubClient_h
//     #include "MQTTConnection.h"
//   #else
// 	 #include "EthernetServerConnection.h"
//   #endif
//
// #endif


// MQTT + ESP8266
#if defined(PubSubClient_h) && defined(ESP8266)
#include "connections/MQTTWifiConnection.h"
#endif

// MQTT + Ethernet Shield / Arduino YUN
#if defined(PubSubClient_h) && ( defined(ethernet_h) || defined(_YUN_CLIENT_H_))
#include <PubSubClient.h>
#include "connections/MQTTEthConnection.h"
#endif


// Arduino YUN Wifi/Ethernet Server
#if defined(_YUN_SERVER_H_) && !defined(PubSubClient_h)
	#include "connections/YunServerConnection.h"
#endif

// ESP8266 AT Command library
#if defined(__ESP8266AT_H__)
	#include <connections/WifiConnetionEspAT.h>
#endif


#if defined(_EL_CLIENT_H_)
	#include <connections/EspLinkConnection.cpp>
#endif

// ESP8266 Standalone
// #if defined(ESP8266) && !defined(PubSubClient_h)
// 	#include "stdlib_noniso.h"
// 	#include <ESP8266WiFi.h>
// 	#include <WifiConnection.h>
// #endif



//================================================
// Features
//================================================


// Enable OTA Updates - loading the firmware to ESP module using Wi-Fi connection
// Disable to reduce flash size or for security reasons
// SEE: https://opendevice.atlassian.net/wiki/pages/viewpage.action?pageId=37519365
#ifdef __ARDUINO_OTA_H
#include <ArduinoOTA.h>
#endif

#ifdef ESP8266HTTPUPDATE_H_ // Enable OTA Updates
#include <ESP8266httpUpdate.h>
#endif

#include "utility/RemoteUpdate.h"

#include "utility/TaskScheduler.h" // Enable support for Task Schedule (Cooperative multitasking)


#if defined(MFRC522_h)
	#include <devices/RFIDSensor.cpp>
#endif

#if defined(_RCSwitch_h)
	#include <devices/RFSensor.h>
#endif

#if defined(IRremote_h)
	#include <devices/IRSensor.cpp>
	#include <devices/IRDevice.cpp>
#endif


//================================================
// Boards and Connections
//================================================

// #include <ESP8266WiFi.h>  // ESP8266 - Standalone
// #include <ESP8266AT.h> // ESP8266 - AT Firmwware
//#include <YunClient.h>
// #include <PubSubClient.h> // Enable MQTT





//#if defined(EnableInterrupt_h)
//#define EI_ARDUINO_INTERRUPTED_PIN
//#define LIBCALL_ENABLEINTERRUPT
//#include <EnableInterrupt.h>
//#endif

//================================================
// Devices
//================================================

//#include <MFRC522.h> // RFID
// #include <RCSwitch.h> // RF 433Mhz

// --- IR DEVICE
//#if defined(ESP8266)
//#include "IRremoteESP8266.h"
//#else
//#include "IRremote.h"
//#endif
// -- END




#endif /* LIBRARIES_OPENDEVICE_SRC_DEPENDENCIES_H_ */
