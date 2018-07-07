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

#ifndef ODevWiFi_h
#define ODevWiFi_h

#include "../WiFiManager/WiFiManager.h"
#include "../config.h"
#include "Timeout.h"


namespace od {

/**
 * Utility class to Configure Wifi and Server parameters
 * Use WiFiManager to create a Self configuration page
 */
class WiFi
{
  public:
    WiFi();
    ~WiFi();

    boolean autoConnect(char const *apPassword = NULL);
    
    void setLed(int led, byte ledOffState);

    /** Set to fase, to use ODev.resetPin to enter in Seup Mode */
    void enableAutoSetup(bool mode);

    /** Allow customize Server, ApiKey, DeviceName */
    void enableServerParams(bool mode);

    /** INTENAL - Used to provide feedback */
    static void onConfigLoop(bool connected);
    
  private:
    const char*   _apPassword             = NULL;
    byte ledOffState; // hold value for OFF state for LED
    bool autoSetup;
    bool serverParams;

    static Timeout blink;
    static int ledPin;

    

    

};

}

#endif
