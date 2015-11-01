#include "../dependencies.h"

#ifdef __ESP8266_H__

#include "ESP8266WifiConfig.h"

ESP8266WiFiClass::ESP8266WiFiClass(){

}


void ESP8266WiFiClass::init(ESP8266 *impl){
	ESP = impl;
}

void ESP8266WiFiClass::mode(WiFiMode mode){
	if(mode == WIFI_STA) ESP->setOprToStation();
	if(mode == WIFI_AP) ESP->setOprToSoftAP();
	if(mode == WIFI_AP_STA) ESP->setOprToStationSoftAP();
}

/* Set up an open access point
 *
 * param ssid: Pointer to the SSID string.
 */
void ESP8266WiFiClass::softAP(const char* ssid){
	// wifi->
}

/* Set up a WPA2-secured access point
 *
 * param ssid: Pointer to the SSID string.
 * param passphrase: Pointer to passphrase, 8 characters min.
 * param channel: WiFi channel number, 1 - 13.
 * param ssid_hidden: Network cloaking? 0 = broadcast SSID, 1 = hide SSID
 */
void ESP8266WiFiClass::softAP(const char* ssid, const char* passphrase, int channel, int ssid_hidden){

}

/**
 * Start Wifi connection
 * if passphrase is set the most secure supported mode will be automatically selected
 * @param ssid const char*          Pointer to the SSID string.
 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
 * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
 * @param channel                   Optional. Channel of AP
 * @return
 */
int ESP8266WiFiClass::begin(const char* ssid, const char *passphrase, int32_t channel, uint8_t bssid[6]){
	return ESP->joinAP(ssid, passphrase);
}

int ESP8266WiFiClass::disconnect(bool wifioff){
	return ESP->leaveAP();
}

ESP8266WiFiClass WiFi;

#endif
