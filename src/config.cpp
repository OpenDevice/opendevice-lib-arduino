#include "config.h"

namespace od {
	ConfigClass Config = {
    CONFIG_VERSION,
    // The default values
    "OpenDevice",
    "cloud.opendevice.io",
    "*", // APP_ID
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // MAC
    { 0, 0, 0, 0 }, // IP
    255, // ResetPIN
    false, // debugMode
    true, // keepAlive
    DEBUG_SERIAL, // debugTarget
    CONNECTION_MODE_SERVER, // ConnectionMode
    0 // devicesLength
    // Device ...
  };


	/**
	 * Load configuration from storage (EEPROM).
	 * Check if exist, if not, use default values
	 **/
	void ConfigClass::load(){
		if(check()){
			EEPROM.get(CONFIG_START, Config);
		} else{
			memset(this->devices, 0, MAX_DEVICE); // initialize defaults as 0
			memset(this->devicesState, 0, MAX_DEVICE); // initialize defaults as 0
			// save(); // init and save defaults
		}

	}

	/**
	 * Check if exist a valid configuration (memory layout) in EEPROM
	 */
	bool ConfigClass::check(){
		return (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
						EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
						EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]);
	}

  /** Save current configuration to storage */
  void ConfigClass::save(){
    EEPROM.put(CONFIG_START, Config);
	#if defined(ESP8266)
		EEPROM.commit();
	#endif
  }

 /**
	 * Clear saved setings
	 */
  void ConfigClass::clear(){

		for (int i = CONFIG_START ; i < CONFIG_START + sizeof(Config) ; i++) {
		  EEPROM.write(i, 0);
		}

		#if defined(ESP8266)
		EEPROM.commit();
		#endif
  }
}
