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

  byte temperature = map(temperatureRead, 0, 1023, 0, 255);
  byte angle = map(angleRead, 0, 1023, 0, 255);
  byte luminosity = map(luminosityRead, 0, 1023, 0, 255);

  // Send them over the wire
  Wire.beginTransmission(8);
  Wire.write(temperature);
  Wire.write(angle);
  Wire.write(luminosity);
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
