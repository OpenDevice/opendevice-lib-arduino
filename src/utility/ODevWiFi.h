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
 * Utility class to Configure Wifi and Server parameters.
 * Use WiFiManager to create a Self configuration page
 * 
 * @author Ricardo JL Rufino
 * @date 27/06/2018
 */
class WiFi
{
  public:
    WiFi();
    ~WiFi();

    boolean autoConnect(char const *apPassword = NULL);
    
    /** Set ping used to provide feedback (blink) for setup mode*/
    void setLed(int led, byte ledOffState);

    /** 
     * Set to fase, to GoTo setup mode ONLY if RESET pressed
     * Use: ODev.resetPin(RESET_PIN);
     */
    void enableAutoSetup(bool mode);

    /** Allow customize Server, ApiKey, DeviceName */
    void enableServerParams(bool mode);

    /** @internal - Used to provide feedback */
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
