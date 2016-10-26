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

//================================================
// Boards and Connections
//================================================

// #include <ESP8266WiFi.h>  // ESP8266 - Standalone
// #include <ESP8266AT.h> // ESP8266 - AT Firmwware
//#include <YunClient.h>
// #include <PubSubClient.h> // Enable MQTT

//================================================
// Features
//================================================

//#include <ArduinoOTA.h> // Enable OTA Updates - loading the firmware to ESP module using Wi-Fi connection
 					    // Disable to reduce flash size or for security reasons
 						// TIP LINUX :: force discovery, run command: avahi-browse _arduino._tcp
//#include <TaskSchedulerDeclarations.h> // Enable suport for Tash Schedule (Cooperative multitasking)

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
