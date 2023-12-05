#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(8);
  Wire.onReceive(doEcho);
  Wire.onRequest(requestEvent);
}

void doEcho(int byteCount) {
  Serial.println(byteCount);
  while (Wire.available()) {
        char c = Wire.read();
  }
}

void requestEvent() {
  char strtest[1000] = {0};
  for (int i = 0; i < 1000; i++) {
    Wire.write(strtest[i]);
  }
}

void loop() {
}
