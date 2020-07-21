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

		Print* out;

		template<typename T, typename M = char> void debug(const T title, const M str, bool newLine = true){

			// if(Config.debugMode){
				// if(Config.debugTarget == 1){
//					deviceConnection->doStart();
//					deviceConnection->print("DB:");
//					deviceConnection->print(str);
//					deviceConnection->doEnd();
				// }else{
					// #if(ENABLE_SERIAL)
					out->print("DB:");
					out->print(title);
					if(str){
						out->print(" :: ");
						if(newLine) out->println(str);
						else out->print(str);
					}else{
						out->println();
					}
					// #endif
				// }
			// }
		}

		template<typename T> void debug(const T title, bool newLine = true){
			char* messasge = nullptr;
			debug(title,messasge,newLine);
		}

		void printLoop(char txt, uint8_t times, bool newLine = true){
			for (uint8_t i = 0; i < times; ++i) {
				out->print(txt);
			}

			if(newLine) out->println();
		}



	//#ifdef __FlashStringHelper
		// template <class T> void debug(const __FlashStringHelper* title, const T str, bool newLine = true){
		// 	out->print("DB:");
		// 	out->print(title);
		// 	if(str){
		// 		out->print(" :: ");
		// 		if(newLine) out->println(str);
		// 		else out->print(str);
		// 	}else{
		// 		out->println();
		// 	}
		// }
	//#endif


		// void debug(const char title[], int val, bool newLine = true){
		// 	#if(DEBUG)
		// 	out->print("DB:");
		// 	out->print(title);
		// 	out->print(" :: ");
		// 	out->print(val, DEC);
		// 	out->println();
		// 	#endif
		// }

	} LoggerClass;

	extern LoggerClass Logger;

}

#endif /* LIBRARIES_OPENDEVICE_SRC_LOGGER_H_ */
