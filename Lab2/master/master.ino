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
  int angleRead = analogRead(LIGHTSENSORPIN);
  int luminosityRead = analogRead(POTENTIOMETERPIN);

  // Send them over the wire
  Wire.beginTransmission(8);
  Wire.write(temperatureRead);
  Wire.write(angleRead);
  Wire.write(luminosityRead);
  Wire.endTransmission();

  delay(2); // for the analog to digital converter to settle
}
