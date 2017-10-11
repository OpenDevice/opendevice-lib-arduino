/**
 * Clear Config
 * @author Ricardo JL Rufino
 * @date 26/11/16
 */
#include <OpenDevice.h>

void setup() {
  Serial.begin(115200);
  ODev.begin();
  ODev.clear();
  while(!Serial){;}
  Serial.println("Clear OK");
}

void loop() {
  ODev.loop();
}
