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

//#include <MFRC522.h> // RFID
// #include <RCSwitch.h> // RF 433Mhz
// #include <ESP8266AT.h> // ESP8266 - AT Firmwware
#include <ESP8266WiFi.h>  // ESP8266 - Standalone


//#if defined(EnableInterrupt_h)
//#define EI_ARDUINO_INTERRUPTED_PIN
//#define LIBCALL_ENABLEINTERRUPT
//#include <EnableInterrupt.h>
//#endif

//#include <TaskSchedulerDeclarations.h>



// --- IR DEVICE
//#if defined(ESP8266)
//#include "IRremoteESP8266.h"
//#else
//#include "IRremote.h"
//#endif
// -- END


#endif /* LIBRARIES_OPENDEVICE_SRC_DEPENDENCIES_H_ */
