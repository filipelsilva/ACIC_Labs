#include <Wire.h>

unsigned long start = 0;
unsigned long finish = 0;

void testRTT() {
  // Wait for the slave to be ready
  delay(5000);
  // Send the bytes, receive the bytes, get the timestamps
  Wire.beginTransmission(8);
  start = millis();
  Wire.write(0xFF);

  Wire.requestFrom(8, 1);
  int mambos = Wire.read();
  finish = millis();
  Serial.print("Mambos: ");
  Serial.println(mambos, HEX);
  Serial.print("RTT (in miliseconds): ");
  Serial.println(start);
  Serial.println(finish);
  Serial.println(finish - start);

  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  testRTT();
}

void loop() {
}
