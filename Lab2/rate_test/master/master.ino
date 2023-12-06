#include <Wire.h>

unsigned long start = 0;
unsigned long finish = 0;

void testRTT() {
  char strtest[32] = {0};
  char str2[32];

  // Wait for the slave to be ready
  delay(5000);

  // Send the bytes, receive the bytes, get the timestamps
  Wire.beginTransmission(8);
  start = micros();
  for (int i = 0; i < 32; i++) {
    Wire.write(strtest[i]);
  }

  Wire.requestFrom(8, 32);
  int i = 0;
  while (Wire.available()) {
        char c = Wire.read();
        str2[i] = c;
        i++;
  }
  finish = micros();
  Serial.print("time (in microseconds): ");
  Serial.println(finish - start);
  Serial.println(start);
  Serial.println(finish);

  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  testRTT();
}

void loop() {
}
