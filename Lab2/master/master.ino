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
  Wire.write(temperatureRead);
  Wire.write(angleRead);
  Wire.write(luminosityRead);
  Wire.endTransmission();

  Serial.print("T: ");
  Serial.print(temperatureRead);
  Serial.print(" | A: ");
  Serial.print(angleRead);
  Serial.print(" | L: ");
  Serial.print(luminosityRead);
  Serial.println();

  delay(2); // for the analog to digital converter to settle
}
