#include <Wire.h>

#define TEMPERATURESENSORPIN A0
#define LIGHTSENSORPIN A1
#define POTENTIOMETERPIN A3

int temperatureSensorMin = 1023;
int temperatureSensorMax = 0;
int angleSensorMin = 1023;
int angleSensorMax = 0;
int luminositySensorMin = 1023;
int luminositySensorMax = 0;

void calibrateSensors() {
  while (millis() < 10000) {
    int temperatureRead = analogRead(TEMPERATURESENSORPIN);
    int angleRead = analogRead(POTENTIOMETERPIN);
    int luminosityRead = analogRead(LIGHTSENSORPIN);

    if (temperatureRead < temperatureSensorMin) {
      temperatureSensorMin = temperatureRead;
    }
    if (temperatureRead > temperatureSensorMax) {
      temperatureSensorMax = temperatureRead;
    }
    if (angleRead < angleSensorMin) {
      angleSensorMin = angleRead;
    }
    if (angleRead > angleSensorMax) {
      angleSensorMax = angleRead;
    }
    if (luminosityRead < luminositySensorMin) {
      luminositySensorMin = luminosityRead;
    }
    if (luminosityRead > luminositySensorMax) {
      luminositySensorMax = luminosityRead;
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  calibrateSensors();
}

void loop() {
  // Read the values from the sensors
  // int temperatureRead = analogRead(TEMPERATURESENSORPIN);
  // int angleRead = analogRead(POTENTIOMETERPIN);
  // int luminosityRead = analogRead(LIGHTSENSORPIN);
  int temperatureRead = map(analogRead(TEMPERATURESENSORPIN), temperatureSensorMin, temperatureSensorMax, 0, 1023);
  int angleRead = map(analogRead(POTENTIOMETERPIN), angleSensorMin, angleSensorMax, 0, 1023);
  int luminosityRead = map(analogRead(LIGHTSENSORPIN), luminositySensorMin, luminositySensorMax, 0, 1023);

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
