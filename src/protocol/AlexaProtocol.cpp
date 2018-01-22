/*
 * AlexaProtocol.cpp
 *
 *  Created on: 10 de out de 2017
 *      Author: Ricardo JL Rufino
 */

#include <protocol/AlexaProtocol.h>

#if defined(ESP8266)

namespace od {

AlexaProtocol::AlexaProtocol()
	: ipMulti(239, 255, 255, 250),
	  numOfDevices(0),
	  discoveryEnabled(false){

}

AlexaProtocol::~AlexaProtocol() {
}

bool AlexaProtocol::begin(){
  boolean state = false;

  if(discoveryEnabled){

	  Serial.println(F("ALEXA: Enabling Discovery ..."));

	  if(UDP.beginMulticast(WiFi.localIP(), ipMulti, portMulti)) {
	    Serial.print(F("ALEXA: Multicast server started at "));
	    Serial.print(ipMulti);
	    Serial.print(":");
	    Serial.println(portMulti);

	    state = true;
	  }
	  else{
	    Serial.println(F("ALEXA: UDP failed"));
	  }

  }


  return state;
}

void AlexaProtocol::addDevice(Device* device) {

  if(device->type == Device::DIGITAL && ! device->sensor){
	  Serial.print(F("ALEXA: Adding device '"));
	  Serial.print(device->name());
	  Serial.print("', index : ");
	  Serial.println(numOfDevices);
	  devices[numOfDevices] = new AlexaDevice(device, numOfDevices);
	  numOfDevices++;
  }

}

void AlexaProtocol::enableDiscovery(bool val){
    discoveryEnabled = val;
}

void AlexaProtocol::loop(){

  if(discoveryEnabled){
	  int packetSize = UDP.parsePacket();
	  if (packetSize > 0)
	  {
	    IPAddress senderIP = UDP.remoteIP();
	    unsigned int senderPort = UDP.remotePort();

	    // read the packet into the buffer
	    UDP.read(packetBuffer, packetSize);

	    // check if this is a M-SEARCH for WeMo device
	    String request = String((char *)packetBuffer);
	    // Serial.println("----------");
	    // Serial.println(request);
	    // Serial.println("-----------");
	    if(request.indexOf('M-SEARCH') > 0) {
	        if((request.indexOf("urn:Belkin:device:**") > 0) || (request.indexOf("ssdp:all") > 0) || (request.indexOf("upnp:rootdevice") > 0)) {
	          Serial.println(F("ALEXA: Got UDP Belkin Request !!!"));

	          // int arrSize = sizeof(switchs) / sizeof(Switch);

	          for(int n = 0; n < numOfDevices; n++) {
	              AlexaDevice* sw = devices[n];

	              if (&sw != NULL) {
	                sw->respondToSearch(senderIP, senderPort);
	              }
	          }
	        }
	    }
	  }
  }


  for(int n = 0; n < numOfDevices; n++) {
      AlexaDevice* sw = devices[n];

      if (&sw != NULL) {
    	  sw->loop();
      }
  }

}

AlexaProtocol Alexa;

} /* namespace od */

#endif
