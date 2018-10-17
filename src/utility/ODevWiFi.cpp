
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

#ifdef ESP8266

#include "ODevWiFi.h"

namespace od {
 
  // Static 
  Timeout WiFi::blink(500, true);
  int WiFi::ledPin = 0;

  WiFi::WiFi() {
    ledOffState = LOW;
    autoSetup = false;
  }

  WiFi::~WiFi(){

  }


  boolean WiFi::autoConnect(char const *apPassword) {

    // Notify iM Alive !!!
    digitalWrite(ledPin, !ledOffState); // ON
    delay(500);
    digitalWrite(ledPin, ledOffState); // OFF

    WiFiManager wifiManager;

    // Configure visual feedback for wifi config
    // wifiManager.setSaveConfigCallback(onStartConfigMode);
    wifiManager.setApLoopCallback(WiFi::onConfigLoop);

    // Params
    // =================

    WiFiManagerParameter custom_server("server", "ODev Server", Config.server, 28);
    WiFiManagerParameter custom_name("name", "Device Name", Config.moduleName, 20);
    WiFiManagerParameter custom_key("key", "APIKey", Config.appID, 28);

    if(serverParams){
      wifiManager.addParameter(&custom_server);
      wifiManager.addParameter(&custom_name);
      wifiManager.addParameter(&custom_key);
    }

    // Allways enter in setup mode
    if(autoSetup){

      // Try connect (if not, goto Ap Mode (blocking loop) )
      if (!wifiManager.autoConnect(Config.moduleName, apPassword)) {
        Serial.println("ODev: Failed to connect and hit timeout (RESETING...)");
        delay(3000);
        ESP.reset(); 
        delay(5000);
      }
    
    // Check if setup pin is pressed !
    }else if (Config.pinReset != 255 && digitalRead(Config.pinReset) == LOW){

      if (!wifiManager.startConfigPortal(Config.moduleName, apPassword)) {
        Serial.println("ODev: Failed to connect and hit timeout (RESETING...)");
        delay(3000);
        ESP.reset(); //reset and try again
        delay(5000);
      }

    }

    // Save params
    if(serverParams){

      strcpy (Config.server, custom_server.getValue());
      strcpy (Config.moduleName, custom_name.getValue());
      strcpy (Config.appID, custom_key.getValue());

      Config.save();

    }

    digitalWrite(ledPin, !ledOffState); // ON

  }
  
  /** Called from WiFiManager */
  void WiFi::onConfigLoop(bool connected){

    if(!connected){
      if(blink.expired()){
        digitalWrite(ledPin, !digitalRead(ledPin));
      }
    }else{
      blink.disable();
    }

  }

  void WiFi::setLed(int pin, byte _ledOffState){
    WiFi::ledPin = pin;
    ledOffState = _ledOffState;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, _ledOffState);
  }

  void WiFi::enableAutoSetup(bool mode){
    autoSetup = mode;
  }

  void WiFi::enableServerParams(bool mode){
    serverParams = mode;
  }
}

#endif
