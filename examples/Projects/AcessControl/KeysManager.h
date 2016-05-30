/**
 * Class to Manage RFID keys in EEPROM
 */

#include <EEPROM.h>
#include <utility/Logger.h> // from OpenDevice

#define MAX_KEYS_STORAGE 50
#define START_ADDRESS 0


struct StorageClass{
  int count;
  unsigned long keys[MAX_KEYS_STORAGE]; // FIXME: WARN: memmory usage....
};

struct KeysManagerType{

  StorageClass storage;
  
  /**
   * Read settings on startup
   */
  void init(){

      #if defined(ESP8266)
        EEPROM.begin(4096);
      #else
        EEPROM.begin(EEPROM.length());
      #endif

      EEPROM.get(START_ADDRESS, storage);
      Logger.debug("KeysManager :: keys", storage.count);
  }

  /**
   * Save new key to storage.
   */
  boolean save(unsigned long key){
    if(storage.count < MAX_KEYS_STORAGE){
      storage.keys[storage.count] = key;
      storage.count++;
      EEPROM.put(START_ADDRESS, storage);
      EEPROM.commit();  
      return true;
    }else{
      return false;
    }
  }

  /**
   * Check if key exist.
   */
  boolean exist(unsigned long key){
      int asize = (sizeof(storage.keys)/sizeof(storage.keys[0]));
      for (int i = 0; i < asize; i++) {
        if (key == storage.keys[i]) return true;
      }
      return false;
  }

  boolean clear(){
    storage.count = 0;
    int asize = (sizeof(storage.keys)/sizeof(storage.keys[0]));
    for (int i = 0; i < asize; i++) {
      storage.keys[i] = 0;
    } 
    
    EEPROM.put(START_ADDRESS, storage);
    EEPROM.commit();   
  }

} KeysManager;

