#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(8);
  Wire.onReceive(doEcho);
  Wire.onRequest(requestEvent);
}

void doEcho(int byteCount) {
  if (byteCount != 1) {
    Serial.println("Not the expected number of bytes");
    return;
  }

  int received = Wire.read();
  Serial.print("Received: ");
  Serial.println(received, HEX);
}

void requestEvent() {
  Wire.write(0xFF);
}

void loop() {
}
