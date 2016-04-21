/*
 * Logger.h
 *
 *  Created on: 01/11/2015
 *      Author: ricardo
 */

#ifndef LIBRARIES_OPENDEVICE_SRC_LOGGER_H_
#define LIBRARIES_OPENDEVICE_SRC_LOGGER_H_

#include "config.h"

namespace od {

typedef struct{

		void debug(const char title[], const char str[] = "", bool newLine = true){
			if(Config.debugMode){
				if(Config.debugTarget == 1){
//					deviceConnection->doStart();
//					deviceConnection->print("DB:");
//					deviceConnection->print(str);
//					deviceConnection->doEnd();
				}else{
					#if(ENABLE_SERIAL)
					Serial.print("DB:");
					Serial.print(title);
					if(str && strlen(str) > 0){
						Serial.print(" :: ");
						if(newLine) Serial.println(str);
						else Serial.print(str);
					}else{
						Serial.println();
					}
					#endif
				}
			}
		}

		void debug(const char title[], int val, bool newLine = true){
			if(Config.debugMode){
				if(Config.debugTarget == 1){
//					deviceConnection->doStart();
//					deviceConnection->print("DB:");
//					deviceConnection->print(str);
//					deviceConnection->doEnd();
				}else{
					#if(ENABLE_SERIAL)
					Serial.print("DB:");
					Serial.print(title);
					Serial.print(" :: ");
					Serial.print(val, DEC);
					Serial.println();
					#endif
				}
			}
		}

	} LoggerClass;

	extern LoggerClass Logger;

}

#endif /* LIBRARIES_OPENDEVICE_SRC_LOGGER_H_ */
