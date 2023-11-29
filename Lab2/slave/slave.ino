#include <Wire.h>

#define LED_BLUE 2
#define LED_RED 3
#define LED_YELLOW 4

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

void setup() {
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(updateLEDs);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
}

void setTemperatureLED(int temperature) {
  if (temperature > 24) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }
}

void setAngleLED(int angle) {
  int msecs = map(angle, 0, 255, 6, 66);
  Serial.print(" | msecs: ");
  Serial.print(msecs);
  if (hasIntervalPassed(msecs)) {
    if (currentAngleLEDState == HIGH) {
      currentAngleLEDState = LOW;
    } else {
      currentAngleLEDState = HIGH;
    }
    digitalWrite(LED_BLUE, currentAngleLEDState);
  }
}

void setLuminosityLED(int luminosity) {
  analogWrite(LED_YELLOW, 255 - luminosity);
}

void updateLEDs(int) {
  // Read the values
  unsigned int temperatureRead = Wire.read();
  Serial.print("T: ");
  Serial.print(temperatureRead, HEX);

  unsigned int angleRead = Wire.read();
  Serial.print(" | A: ");
  Serial.print(angleRead, HEX);

  unsigned int luminosityRead = Wire.read();
  Serial.print(" | L: ");
  Serial.print(luminosityRead, HEX);

  // Map the values to the "real" scale
  int temperature = (((temperatureRead / 256.0) * 5.0 ) - 0.5 ) * 100;
  int angle = angleRead;
  int luminosity = luminosityRead;

  // Update LEDs
  setTemperatureLED(temperature);
  setAngleLED(angle);
  setLuminosityLED(luminosity);

  Serial.println();
}

void loop() {
}
