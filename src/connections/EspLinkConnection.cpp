/*
 * EspLinkConnection.cpp
 *
 *  Created on: 20 de nov de 2016
 *      Author: ricardo
 */

#if defined(_EL_CLIENT_H_)

#include "EspLinkConnection.h"

namespace od {

ELClient* EspLinkConnection::esp;
ELClientCmd* EspLinkConnection::cmd;
ELClientMqtt* EspLinkConnection::mqtt;
StreamBuffer* EspLinkConnection::buffer;
bool EspLinkConnection::connected = false;
bool EspLinkConnection::received = false;

EspLinkConnection::EspLinkConnection(Stream& serial, Stream& debug){
	esp = new ELClient(&serial, &debug);
	init(esp);
}

EspLinkConnection::EspLinkConnection(Stream& serial){
	esp = new ELClient(&serial);
	init(esp);
}

EspLinkConnection::EspLinkConnection(ELClient& _esp) {
	init(&_esp);
}

EspLinkConnection::~EspLinkConnection() {

}

void EspLinkConnection::init(ELClient* _esp){
	esp = _esp;
	cmd = new ELClientCmd(_esp);
	mqtt = new ELClientMqtt(_esp);
	buffer = new StreamBuffer(_buffer, DATA_BUFFER);
}

void EspLinkConnection::wifiCb(void* response) {
	ELClientResponse *res = (ELClientResponse*) response;
	if (res->argc() == 1) {
		uint8_t status;
		res->popArg(&status, 1);

		if (status == STATION_GOT_IP) {
			LOG_DEBUG_S("WIFI CONNECTED");
		} else {
			LOG_DEBUG("WIFI NOT READY", status);
		}
	}
}

// Callback when MQTT is connected
void EspLinkConnection::mqttConnected(void* response) {
	esp->DBG("MQTT connected!");

	String subs = String(Config.appID);
	subs += "/in/";
	subs += Config.moduleName;

	mqtt->subscribe(subs.c_str());
	connected = true;
}

// Callback when MQTT is disconnected
void EspLinkConnection::mqttDisconnected(void* response) {
	esp->DBG("MQTT disconnected");
	connected = false;
}

// Callback when an MQTT message arrives for one of our subscriptions
void EspLinkConnection::mqttData(void* response) {
	ELClientResponse *res = (ELClientResponse *) response;

	res->popString(); // topic

	String data = res->popString();
	LOG_DEBUG("MQTT REC", data.c_str());

	buffer->flush();
	received = true;
	for (uint32_t i = 0; i < data.length(); i++) {
		buffer->write(data[i]);
	}
}

size_t EspLinkConnection::write(uint8_t v){
	if(v == Command::ACK_BIT){ // don't write ACK
		#if DEBUG_CON
		LOG_DEBUG("MQTT SEND", (const char *) buffer->_buffer);
		#endif
		mqtt->publish(topic.c_str(), (const char *) buffer->_buffer);
		flush();
		return 1;
	}else{ // Write to buffer
		return buffer->write(v);
	}
}


void EspLinkConnection::begin() {
	esp->wifiCb.attach(&(EspLinkConnection::wifiCb)); // wifi status change callback, optional (delete if not desired)
	bool ok;
	do {
		ok = esp->Sync();   // sync up with esp-link, blocks for up to 2 seconds
		if (!ok) esp->DBG("EL-Client sync failed! ");
	} while (!ok);


	// Set-up callbacks for events and initialize with es-link.
	mqtt->connectedCb.attach(&(EspLinkConnection::mqttConnected));
	mqtt->disconnectedCb.attach(&(EspLinkConnection::mqttDisconnected));
	mqtt->dataCb.attach(&(EspLinkConnection::mqttData));
	mqtt->setup();

	//  Publish topic
	topic = String(Config.appID);
	topic += "/out/";
	topic += Config.moduleName;

	esp->DBG("EL-MQTT ready");
}

bool EspLinkConnection::checkDataAvalible() {
	esp->Process();

	if(received){
		setStream(buffer);
		DeviceConnection::checkDataAvalible();
		received = false;
	}

	return false;
}

}

#endif
