/*
 * OpenDeviceTiny.h
 *
 *  Created on: 21/07/2015
 *      Author: ricardo
 */

#ifndef OPENDEVICETINY_H_
#define OPENDEVICETINY_H_

#include "Command.h"
#include "Device.h"
#include "DeviceConnection.h"

class OpenDeviceTiny {

private:

	uint8_t devices[MAX_DEVICE];
	uint8_t deviceLength;
	DeviceConnection *deviceConnection;

	// Internal Listeners..
	// NOTE: Static because: deviceConnection->setDefaultListener
	static void onMessageReceived(Command cmd);

	void notifyReceived(ResponseStatus::ResponseStatus status);

	void _loop();
	void _begin();


public:
	OpenDeviceTiny();
	virtual ~OpenDeviceTiny();

	void loop(){
		_loop();
	};

	void begin(DeviceConnection &deviceConnection);

	// For: ATtiny/Digispark
	#if defined(SoftSerial_h)
		void begin(unsigned long baud, SoftSerial &soft){
			soft.begin(baud);
			//Serial.begin(baud);
			DeviceConnection *conn =  new DeviceConnection(soft);
			begin(*conn);
		}
	#endif

	// For: ATtiny/Digispark
	#if defined(DEFAULT_TO_TINY_DEBUG_SERIAL)
		void begin(TinyDebugSerial &serial, unsigned long baud);
	#endif


	bool addDevice(uint8_t pin, Device::DeviceType type);
	uint8_t getDevice(uint8_t);
};



extern OpenDeviceTiny ODevTiny;

#endif /* OPENDEVICETINY_H_ */
