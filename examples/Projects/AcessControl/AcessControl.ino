/*
 * Security System and Access Control using RFID and OpenDevice
 *
 * See docs and schematics: 
 * https://opendevice.atlassian.net/wiki/display/DOC/Access+Control+using+RFID
 * 
 * The project allows you to run both offline and online.
 * In Off-line mode, the user can register new keys using the master key, 
 * new keys will be saved in the device EEPROM.
 * On-line mode allows use of a larger number of keys as well as a more intelligent access control.
 * NOTE: two modes can be used simultaneously
 * 
 * Required Libraries:
 * - ESP8266WiFi  (for ESP8266WiFi)
 * - MFRC522
 * - PubSubClient
 * - ArduinoOTA (Optional, if you need remote updates)
 * 
 * 
 * @author: Ricardo JL Rufino
 * @version 1.1 (29/05/16)
 * 
 * 
 * ***** [ OpenDevice ] **************************************************
 * OpenDevice is a set of tools and APIs to build solutions for the 
 * "Internet of Things" like home automations systems, robotics, 
 * smart city, energy monitoring, security, sensor monitoring.
 * Provide an open and unified framework to interconnect IoT devices, 
 * data, and applications over the Web, if you need communication 
 * between software and hardware, probably your place is here.
 * 
 * NOTE: You must enable the libraries used in: 
 * File: "{arduinolibs}/OpenDevice/dependencies.h"               
 * Check: https://github.com/OpenDevice/opendevice-lib-arduino/issues/6
 * 
 * See docs: http://opendevice.io
 * https://opendevice.atlassian.net/wiki/display/DOC/Getting+started
 * 
 * ***********************************************************************
 */

 
//#include <MFRC522.h>
//#include <YunServer.h> // Enable for Arduino YUN (as TCPServer)
//#include <YunClient.h> // Enable for MQTT/Yun (as MQTT Client)


#include <TaskScheduler.h>

#include <OpenDevice.h>

#include "KeysManager.h"

// WiFi Config
const char* ssid = "---";
const char* password = "---";

// Master keys, used to add new keys to device
// NOTE: Pass the master tag twice, clear the memory
const unsigned long masterkeys[] = {993104189}; 


#define ENABLE_SERVER_AUTH  true // Use second level (server) autentication
                                 // In addiction to internal autentication

// PINs
#define PIN_BEEP 0  
#define PIN_DOOR 15
//#define PIN_DOOR 4 // only for test
#define PIN_RESET 2 // NOT WORK ON ESP.  
#define LED_SUCCESS 16
#define LED_FAIL 4

// RFID Sensor PINS
#define RFID_RST   2   // MFRC522 Reset
#define RFID_SS    5   // MFRC522 SDA (SPI SS)

// Beep Notification (Tones)
#define NOTE_E3  165
#define NOTE_C8  4186

#define LOCK_TYPE_PULSE 0  // electric locks activated / released using pulses
#define LOCK_TYPE_CONTINUOUS 1 // electric locks that are opened using a continuous pulse

// lockType defines the type of electric lock used
const byte lockType = LOCK_TYPE_CONTINUOUS;  

bool addKeyMode = false; // (do not change) configuration that defines the registration of new keys 

// Static Devices definition
RFIDSensor rfid(RFID_SS, RFID_RST);
Device lock(PIN_DOOR, Device::DIGITAL);

//Tasks Definitions
Task authTimeoutTask(3000, TASK_FOREVER);
Task lockCloseTask(5000, TASK_FOREVER);
Task blinkTask(150, TASK_FOREVER);


void setup() {
  ODev.name("RFIDAccess");
  Config.server = "192.168.3.105"; // 
  Config.pinReset = PIN_RESET;
  ODev.enableDebug();
  
  ODev.addSensor(rfid)->onChange(onRFIDChange); // ID:1
  ODev.addDevice("Lock", lock)->onChange(onLockOpen); // ID:2
  
  ODev.addCommand("onAuthFinish", onAuthFinish);
  ODev.addCommand("enableAddKey", enableAddKey);

  ODev.addTask(lockCloseTask, lockClose); // Fire: lockClose
  ODev.addTask(authTimeoutTask, onAuthFail);
  ODev.addTask(blinkTask, blinkHandler);

  // Local pin config
  pinMode(PIN_BEEP, OUTPUT);
  pinMode(LED_SUCCESS, OUTPUT);
  pinMode(LED_FAIL, OUTPUT);

  KeysManager.init();
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  ODev.begin(WiFi);
  
  // Notify start
  digitalWrite(LED_SUCCESS, HIGH);
  tone(PIN_BEEP, NOTE_C8, 400);
  delay(1000);
  digitalWrite(LED_SUCCESS, LOW);
  digitalWrite(LED_FAIL, LOW);
  noTone(PIN_BEEP); 
}

void loop() {
  
  ODev.loop();

}

bool onRFIDChange(uint8_t id, unsigned long value) {

  // Add key Mode enabled
  if(addKeyMode){

    // twice, clear the memory
    if(isMaster(value)){
      clearKeys();
    }else{
      saveKey(value);
    }

    return false; // not send to server for now
  }

  if(ENABLE_SERVER_AUTH){
    // Used to wait server autentication (optional)
    authTimeoutTask.enableDelayed();  
    // Enable blink task
    blinkTask.enableDelayed();

    digitalWrite(LED_SUCCESS, HIGH); // inital state for blink
  }
  
  tone(PIN_BEEP, NOTE_C8, 100);
  
  if (isMaster(value)) {
    enableAddKey(); // start add key mode.
  // Check on local storage (ex.: EEPROM)
  } else if(KeysManager.exist(value)){
    onAuthSuccess();
  } else if(!ENABLE_SERVER_AUTH){
    onAuthFail();
  } else{
    
  }
  
  return true; // continue, send to server and wait response
}

// Called by Server
void onAuthFinish(){
  authTimeoutTask.disable();

  int authStatus = ODev.readInt(); 
  ODev.debug("onAuthFinish:", authStatus);

  if(authStatus > 0) onAuthSuccess();
  else onAuthFail();
}

void onAuthSuccess(){
  
  noTone(PIN_BEEP);
  
  authTimeoutTask.disable();
  blinkTask.disable();
  
  lock.on(); // Fire: onLockOpen

}



// Called by Task on timeout or by Server
void onAuthFail(){
  authTimeoutTask.disable();
  blinkTask.disable();
  addKeyMode = false;
  
  tone(PIN_BEEP, NOTE_E3, 1000);

  digitalWrite(LED_FAIL, HIGH);
  digitalWrite(LED_SUCCESS, LOW);
  delay(1500);
  digitalWrite(LED_FAIL, LOW);
  noTone(PIN_BEEP);
}

/** Called by lockCloseTask */
void lockClose(){

  lockCloseTask.disable(); // stop wait task
  
  digitalWrite(LED_SUCCESS, LOW);
  digitalWrite(LED_FAIL, LOW);
  lock.off();
}

/** 
 * Called Local (using lock.on()) or by Server 
 */
bool onLockOpen(byte id, unsigned long value){

  if(value == HIGH){
    digitalWrite(LED_SUCCESS, HIGH);
    digitalWrite(LED_FAIL, LOW);
    tone(PIN_BEEP, NOTE_C8, 400); 

    if(lockType == LOCK_TYPE_CONTINUOUS){
      // Using task to avoid normal delay (and WDT reset)
      lockCloseTask.enableDelayed(); // Fire: lockClose()  
    }else if (lockType == LOCK_TYPE_PULSE){ 
      // Send pulses to force release lock
      for(int i = 0; i <= 4; i++){
        delay(250);  
        digitalWrite(PIN_DOOR, !digitalRead(PIN_DOOR));
      }
      lockCloseTask.enableDelayed(); // this is used only to send event to server
    }
   
  }

  return true;
}

/** Called by blinkTask */
void blinkHandler(){
  digitalWrite(LED_SUCCESS, !digitalRead(LED_SUCCESS));
  digitalWrite(LED_FAIL,!digitalRead(LED_FAIL));
}

void beep(byte count, int tonef, int toneDelay, int stopDelay){
  for (int i = 0; i < count; i++){
    tone(PIN_BEEP, tonef, toneDelay); delay(stopDelay);
  }
  noTone(PIN_BEEP);
}


/**
 * Offline Validation
 */
bool isMaster(unsigned long value) {
  int asize = (sizeof(masterkeys)/sizeof(masterkeys[0]));
  for (int i = 0; i < asize; i++) {
    if (value == masterkeys[i]) return true;
  }
  return false;
}

/** 
Called from server to add new key ou using master keys
When the next rfid tag is detected, it will run the method: saveKey
*/
void enableAddKey(){
  addKeyMode = true; // put in add key mode.
  authTimeoutTask.enableDelayed(); // wait for next key with delay (call onAuthFail )
  blinkTask.enableDelayed();

  digitalWrite(LED_SUCCESS, HIGH);
  digitalWrite(LED_FAIL, HIGH);
  beep(3, NOTE_C8, 100, 200);    
}

/**
 * Save key using KeysManager.
 */
void saveKey(unsigned long key){
  addKeyMode = false;
  blinkTask.disable();
  authTimeoutTask.disable();

  bool s = KeysManager.save(key);;
  
  if(!s){ // FAIL
      digitalWrite(LED_SUCCESS, LOW);
      digitalWrite(LED_FAIL, HIGH);
      tone(PIN_BEEP, NOTE_E3, 1000); delay(200);
      noTone(PIN_BEEP);
  }else{
      beep(3, NOTE_C8, 100, 200);    
      digitalWrite(LED_SUCCESS, LOW);
      digitalWrite(LED_FAIL, LOW);

      // TODO: send to server ?
  }

}

/**
 * Clear keys from storage
 */
void clearKeys(){
  addKeyMode = false;
  blinkTask.disable();
  authTimeoutTask.disable();
  
  KeysManager.clear(); 
  
  beep(5, NOTE_E3, 100, 200);
  digitalWrite(LED_SUCCESS, LOW);
  digitalWrite(LED_FAIL, LOW);
}

