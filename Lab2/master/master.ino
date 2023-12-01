#include <Wire.h>

#define TEMPERATURESENSORPIN A0
#define LIGHTSENSORPIN A1
#define POTENTIOMETERPIN A3

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  // Read the values from the sensors
  int temperatureRead = analogRead(TEMPERATURESENSORPIN);
  int angleRead = analogRead(POTENTIOMETERPIN);
  int luminosityRead = analogRead(LIGHTSENSORPIN);

  // Send them over the wire
  Wire.beginTransmission(8);
  Wire.write(temperatureRead >> 8);
  Wire.write(temperatureRead & 255);
  Wire.write(angleRead >> 8);
  Wire.write(angleRead & 255);
  Wire.write(luminosityRead >> 8);
  Wire.write(luminosityRead & 255);
  Wire.endTransmission();

  Serial.print("T: ");
  Serial.print(temperatureRead, HEX);
  Serial.print(" | A: ");
  Serial.print(angleRead, HEX);
  Serial.print(" | L: ");
  Serial.print(luminosityRead, HEX);
  Serial.println();

  delay(2); // for the analog to digital converter to settle
}
