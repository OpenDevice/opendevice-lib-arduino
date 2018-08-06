/*
 * Logger.h
 *
 *  Created on: 01/11/2015
 *      Author: ricardo
 */



#ifndef LIBRARIES_OPENDEVICE_SRC_LOGGER_H_
#define LIBRARIES_OPENDEVICE_SRC_LOGGER_H_

#include "Arduino.h"
#include "config.h"

// Workaround issue: https://github.com/esp8266/Arduino/issues/2078
#ifdef ESP8266
	#undef PROGMEM
	#define PROGMEM
	#undef PSTR
	#define PSTR(s) (s)
	#undef pgm_read_byte
	#define pgm_read_byte(addr) (*reinterpret_cast<const uint8_t*>(addr))
	#undef pgm_read_word
	#define pgm_read_word(addr) (*reinterpret_cast<const uint16_t*>(addr))
#endif

#if DEBUG || defined(DEBUG_ESP_PORT)
#define LOG_DEBUG(...) Logger.debug(__VA_ARGS__)
#define LOG_DEBUG_S(...) Logger.debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#define LOG_DEBUG_S(...)
#endif


namespace od {

typedef struct{

		template<typename T, typename M = char> void debug(const T title, const M str, bool newLine = true){

			// if(Config.debugMode){
				// if(Config.debugTarget == 1){
//					deviceConnection->doStart();
//					deviceConnection->print("DB:");
//					deviceConnection->print(str);
//					deviceConnection->doEnd();
				// }else{
					// #if(ENABLE_SERIAL)
					Serial.print("DB:");
					Serial.print(title);
					if(str){
						Serial.print(" :: ");
						if(newLine) Serial.println(str);
						else Serial.print(str);
					}else{
						Serial.println();
					}
					// #endif
				// }
			// }
		}

		template<typename T> void debug(const T title, bool newLine = true){
			char* messasge = nullptr;
			debug(title,messasge,newLine);
		}

		void printLoop(char txt, uint8_t times){
			for (uint8_t i = 0; i < times; ++i) {
				Serial.print(txt);
			}

			Serial.println();
		}



	//#ifdef __FlashStringHelper
		// template <class T> void debug(const __FlashStringHelper* title, const T str, bool newLine = true){
		// 	Serial.print("DB:");
		// 	Serial.print(title);
		// 	if(str){
		// 		Serial.print(" :: ");
		// 		if(newLine) Serial.println(str);
		// 		else Serial.print(str);
		// 	}else{
		// 		Serial.println();
		// 	}
		// }
	//#endif


		// void debug(const char title[], int val, bool newLine = true){
		// 	#if(DEBUG)
		// 	Serial.print("DB:");
		// 	Serial.print(title);
		// 	Serial.print(" :: ");
		// 	Serial.print(val, DEC);
		// 	Serial.println();
		// 	#endif
		// }

	} LoggerClass;

	extern LoggerClass Logger;

}

#endif /* LIBRARIES_OPENDEVICE_SRC_LOGGER_H_ */
