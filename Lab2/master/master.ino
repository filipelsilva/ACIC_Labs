#include <Wire.h>

#define LED_BLUE 2
#define TEMPERATURESENSORPIN A0
#define LIGHTSENSORPIN A1

unsigned long previousTime = 0;
int currentAngleLEDState = LOW;

bool hasIntervalPassed(int interval) {
  unsigned long now = millis();

  if (now >= previousTime + interval) {
    previousTime = now;
    return true;
  }

  return false;
}

void setAngleLED(int angle) {
  int msecs = map(angle, 0, 180, 2000, 200);
  if (hasIntervalPassed(msecs)) {
    if (currentAngleLEDState == HIGH) {
      currentAngleLEDState = LOW;
    } else {
      currentAngleLEDState = HIGH;
    }
    digitalWrite(LED_BLUE, currentAngleLEDState);
  }
}

int luminositySensorMin = 1023;
int luminositySensorMax = 0;

void calibrateSensors() {
  while (millis() < 10000) {
    int luminosityRead = analogRead(LIGHTSENSORPIN);
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
  pinMode(LED_BLUE, OUTPUT);
  // calibrateSensors();
}

void loop() {
  // Read the values from the sensors
  int temperatureRead = analogRead(TEMPERATURESENSORPIN);
  int luminosityRead = map(analogRead(LIGHTSENSORPIN), luminositySensorMin, luminositySensorMax, 0, 1023);

  Wire.beginTransmission(8);

  // Variable delay
  // delay(map(angleRead, 0, 1023, 1000, 0));

  // Send them over the wire
  Wire.write(temperatureRead >> 8);
  Wire.write(temperatureRead & 255);
  Wire.write(luminosityRead >> 8);
  Wire.write(luminosityRead & 255);

  delay(10);
  Wire.requestFrom(8, 2);

  int angleRead = Wire.read() << 8;
  angleRead |= Wire.read();

  Wire.endTransmission();

  int angle = map(angleRead, 0, 1023, 0, 180);
  setAngleLED(angle);

  Serial.print("T: ");
  Serial.print(temperatureRead, HEX);
  Serial.print(" | A: ");
  Serial.print(angleRead, HEX);
  Serial.print(" | L: ");
  Serial.print(luminosityRead, HEX);
  Serial.println();

  delay(2); // for the analog to digital converter to settle
}
