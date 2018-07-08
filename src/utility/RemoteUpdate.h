#ifndef LIBRARIES_OPENDEVICE_SRC_RemoteUpdateClass_H_
#define LIBRARIES_OPENDEVICE_SRC_RemoteUpdateClass_H_

#include "config.h"
#include "utility/Logger.h"

namespace od {

class RemoteUpdateClass
{
  public:

	   RemoteUpdateClass();

    ~RemoteUpdateClass();

    void begin(){

      #ifdef __ARDUINO_OTA_H

      LOG_DEBUG_S("OTA Enabled");

      ArduinoOTA.setHostname(Config.moduleName);

      ArduinoOTA.setPassword(Config.appID);

      ArduinoOTA.onStart([]() {
          LOG_DEBUG_S("OTA Start");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          LOG_DEBUG("Progress", (progress / (total / 100)));
        });

      ArduinoOTA.onError([](ota_error_t error) {
	  # ifdef DEBUG
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
      #endif
      });

      ArduinoOTA.begin();

      #endif

    }

    void check(){
      #ifdef __ARDUINO_OTA_H
      		ArduinoOTA.handle();
      #endif
    }

    boolean updateFromURL(String path){

      LOG_DEBUG("OTA", path);

      #ifdef ESP8266HTTPUPDATE_H_

        ESPhttpUpdate.rebootOnUpdate(false);

        t_httpUpdate_return ret = ESPhttpUpdate.update(path); // .c_str()
        switch(ret) {
            case HTTP_UPDATE_FAILED:
              LOG_DEBUG("OTA", ESPhttpUpdate.getLastErrorString());
              return false;
            case HTTP_UPDATE_NO_UPDATES:
              LOG_DEBUG("OTA","no Update.");
              return false;
            case HTTP_UPDATE_OK:
              LOG_DEBUG("OTA","Update ok."); // may not called we reboot the ESP
              return true;
        }
      #endif

      return false;
    }
};

extern RemoteUpdateClass RemoteUpdate;

} // name-space
#endif
