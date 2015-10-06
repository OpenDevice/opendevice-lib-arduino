#include <OpenDevice.h>

void setup(){
  ODev.addCommand("alertMode", alertMode);
  ODev.begin();
}

void loop(){
  ODev.loop();
}

void alertMode(){
  ODev.debug(ODev.readString());
  int count = ODev.readInt();
  for (int i = 0; i < count; i++) {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);
    delay(200);
  }
  digitalWrite(13, HIGH);
}
