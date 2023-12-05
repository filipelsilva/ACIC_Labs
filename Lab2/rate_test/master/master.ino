#include <Wire.h>

unsigned long start = 0;
unsigned long finish = 0;

void testRTT() {
  char strtest[1000] = {0};
  char str2[1000];

  // Wait for the slave to be ready
  delay(5000);

  // Send the bytes, receive the bytes, get the timestamps
  Wire.beginTransmission(8);
  start = millis();
  for (int i = 0; i < 1000; i++) {
    Wire.write(strtest[i]);
  }

  Wire.requestFrom(8, 1000);
  int i = 0;
  while (Wire.available()) {
        char c = Wire.read();
        str2[i] = c;
        i++;
  }
  finish = millis();
  Serial.print("time (in miliseconds): ");
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
