/*
 * AlexaDevice.cpp
 *
 *  Created on: 10 de out de 2017
 *      Author: Ricardo JL Rufino
 */

#include <protocol/AlexaDevice.h>

#if defined(ESP8266)

namespace od {

AlexaDevice::AlexaDevice(Device* device, unsigned int index) {
	uint32_t uniqueSwitchId = ESP.getChipId() + index;
	char uuid[64];
	sprintf_P(uuid, PSTR("38323636-4558-4dda-9188-cda0e6%02x%02x%02x"),
		  (uint16_t) ((uniqueSwitchId >> 16) & 0xff),
		  (uint16_t) ((uniqueSwitchId >>  8) & 0xff),
		  (uint16_t)   uniqueSwitchId        & 0xff);

	this->device = device;
	serial = String(uuid);
	persistent_uuid = "Socket-1_0-" + serial+"-"+ String(index);

	localPort = ALEXA_START_PORT + index;

	startWebServer();

}

AlexaDevice::~AlexaDevice() {

}


void AlexaDevice::startWebServer(){

  server = new ESP8266WebServer(localPort);

  server->on("/", [&]() {
    handleRoot();
  });


  server->on("/setup.xml", [&]() {
    handleSetupXml();
  });

  server->on("/upnp/control/basicevent1", [&]() {
    handleUpnpControl();
  });

  server->on("/eventservice.xml", [&]() {
    handleEventservice();
  });

  //server->onNotFound(handleNotFound);
  server->begin();

  Logger.debug(F("ALEXA : Start device on port = "), localPort);
}

void AlexaDevice::loop(){
    if (server != NULL) {
        server->handleClient();
        delay(1);
    }
}


void AlexaDevice::handleEventservice(){
  Logger.debug(F("ALEXA: ### Responding to eventservice.xml ###"), "");

  String eventservice_xml = "<scpd xmlns=\"urn:Belkin:service-1-0\">"
        "<actionList>"
          "<action>"
            "<name>SetBinaryState</name>"
            "<argumentList>"
              "<argument>"
                "<retval/>"
                "<name>BinaryState</name>"
                "<relatedStateVariable>BinaryState</relatedStateVariable>"
                "<direction>in</direction>"
              "</argument>"
            "</argumentList>"
          "</action>"
          "</actionList>"
             "<serviceStateTable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>BinaryState</name>"
                "<dataType>Boolean</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
              "<stateVariable sendEvents=\"yes\">"
                "<name>level</name>"
                "<dataType>string</dataType>"
                "<defaultValue>0</defaultValue>"
              "</stateVariable>"
            "</serviceStateTable>"
        "</scpd>\r\n"
        "\r\n";

    server->send(200, "text/plain", eventservice_xml.c_str());
}

void AlexaDevice::handleUpnpControl(){
  Logger.debug(F("ALEXA: ### Responding to /upnp/control/basicevent1 ###"), "");

  String request = server->arg(0);

#if DEBUG_CON
  Serial.print("request:");
  Serial.println(request);
#endif

  String response_xml = "";

  if(request.indexOf("<BinaryState>1</BinaryState>") > 0) {

      device->on();
      response_xml =  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                        "<s:Body>"
                          "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">"
                            "<BinaryState>1</BinaryState>"
                          "</u:SetBinaryStateResponse>"
                        "</s:Body>"
                      "</s:Envelope>\r\n"
                      "\r\n";
  }

  if(request.indexOf("<BinaryState>0</BinaryState>") > 0) {

      device->off();
      response_xml =  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                        "<s:Body>"
                          "<u:SetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">"
                            "<BinaryState>0</BinaryState>"
                          "</u:SetBinaryStateResponse>"
                        "</s:Body>"
                      "</s:Envelope>\r\n"
                      "\r\n";
  }

  server->send(200, "text/xml", response_xml.c_str());

#if DEBUG_CON
  Serial.print("Sending :");
  Serial.println(response_xml);
#endif

}

void AlexaDevice::handleRoot(){
  server->send(200, "text/plain", "You should tell Alexa to discover devices");
}

void AlexaDevice::handleSetupXml(){
  Logger.debug(F("ALEXA: ### Responding to /setup.xml ###"), "");

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

   String setup_xml = "<?xml version=\"1.0\"?>"
         "<root xmlns=\"urn:Belkin:device-1-0\">"
           "<specVersion>"
           "<major>1</major>"
           "<minor>0</minor>"
           "</specVersion>"
           "<device>"
             "<deviceType>urn:Belkin:device:controllee:1</deviceType>"
             "<friendlyName>"+ getAlexaInvokeName() +"</friendlyName>"
             "<manufacturer>Belkin International Inc.</manufacturer>"
             "<modelName>Emulated Socket</modelName>"
             "<modelNumber>3.1415</modelNumber>"
             "<manufacturerURL>http://www.belkin.com</manufacturerURL>"
             "<modelDescription>Belkin Plugin Socket 1.0</modelDescription>"
             "<modelURL>http://www.belkin.com/plugin/</modelURL>"
             "<UDN>uuid:"+ persistent_uuid +"</UDN>"
             "<serialNumber>"+ serial +"</serialNumber>"
             "<binaryState>0</binaryState>"
             "<serviceList>"
               "<service>"
                 "<serviceType>urn:Belkin:service:basicevent:1</serviceType>"
                 "<serviceId>urn:Belkin:serviceId:basicevent1</serviceId>"
                 "<controlURL>/upnp/control/basicevent1</controlURL>"
                 "<eventSubURL>/upnp/event/basicevent1</eventSubURL>"
                 "<SCPDURL>/eventservice.xml</SCPDURL>"
               "</service>"
             "</serviceList>"
           "</device>"
         "</root>\r\n"
         "\r\n";

    server->send(200, "text/xml", setup_xml.c_str());

	#if DEBUG_CON
		Serial.print("Sending :");
		Serial.println(setup_xml);
	#endif

}

String AlexaDevice::getAlexaInvokeName() {
    return device->name();
}

void AlexaDevice::respondToSearch(IPAddress& senderIP, unsigned int senderPort) {

#if DEBUG_CON
	  Serial.println("");
	  Serial.print("Sending response to ");
	  Serial.println(senderIP);
	  Serial.print("Port : ");
	  Serial.println(senderPort);
#endif

  IPAddress localIP = WiFi.localIP();
  char s[16];
  sprintf(s, "%d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);

  String response =
       "HTTP/1.1 200 OK\r\n"
       "CACHE-CONTROL: max-age=86400\r\n"
       "DATE: Sat, 26 Nov 2016 04:56:29 GMT\r\n"
       "EXT:\r\n"
       "LOCATION: http://" + String(s) + ":" + String(localPort) + "/setup.xml\r\n"
       "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
       "01-NLS: b9200ebb-736d-4b93-bf03-835149d13983\r\n"
       "SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
       "ST: urn:Belkin:device:**\r\n"
       "USN: uuid:" + persistent_uuid + "::urn:Belkin:device:**\r\n"
       "X-User-Agent: redsonic\r\n\r\n";

  UDP.beginPacket(senderIP, senderPort);
  UDP.write(response.c_str());
  UDP.endPacket();

}

} /* namespace od */

#endif
