
#ifndef ESP8266WifiConfig_h
#define ESP8266WifiConfig_h

#include <ESP8266.h>

enum WiFiMode { WIFI_OFF = 0, WIFI_STA = 1, WIFI_AP = 2, WIFI_AP_STA = 3 };

class ESP8266WiFiClass{

public:

	ESP8266WiFiClass();

    ESP8266 *ESP;

    void init(ESP8266 *impl);


    void mode(WiFiMode mode);

    /* Set up an open access point
     *
     * param ssid: Pointer to the SSID string.
     */
    void softAP(const char* ssid);

    /* Set up a WPA2-secured access point
	 *
	 * param ssid: Pointer to the SSID string.
	 * param passphrase: Pointer to passphrase, 8 characters min.
	 * param channel: WiFi channel number, 1 - 13.
	 * param ssid_hidden: Network cloaking? 0 = broadcast SSID, 1 = hide SSID
	 */
	void softAP(const char* ssid, const char* passphrase, int channel = 1, int ssid_hidden = 0);

	/**
	 * Start Wifi connection
	 * if passphrase is set the most secure supported mode will be automatically selected
	 * @param ssid const char*          Pointer to the SSID string.
	 * @param passphrase const char *   Optional. Passphrase. Valid characters in a passphrase must be between ASCII 32-126 (decimal).
	 * @param bssid uint8_t[6]          Optional. BSSID / MAC of AP
	 * @param channel                   Optional. Channel of AP
	 * @return
	 */
	int begin(const char* ssid, const char *passphrase = NULL, int32_t channel = 0, uint8_t bssid[6] = NULL);

	int disconnect(bool wifioff = false);


};

extern ESP8266WiFiClass WiFi;

#endif
