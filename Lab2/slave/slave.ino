#include <Wire.h>

#define LED_BLUE 2
#define LED_RED 3
#define LED_YELLOW 3

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
  float seconds = map(angle, 0, 180, 2, 20) / 10;
  if (hasIntervalPassed(seconds)) {
	currentAngleLEDState = !currentAngleLEDState;
	digitalWrite(LED_BLUE, currentAngleLEDState);
  }
}

void setLuminosityLED(int luminosity) {
  analogWrite(LED_YELLOW, luminosity);
}

void updateLEDs(int) {
  // Read the values
  unsigned char mambos[2];
  mambos[1] = Wire.read();
  mambos[0] = Wire.read();
  unsigned int temperatureRead = (unsigned int)mambos;
  mambos[1] = Wire.read();
  mambos[0] = Wire.read();
  unsigned int angleRead = (unsigned int)mambos;
  mambos[1] = Wire.read();
  mambos[0] = Wire.read();
  unsigned int luminosityRead = (unsigned int)mambos;

  // Map the values to the "real" scale
  int temperature = (((temperatureRead / 1024.0) * 5.0 ) - 0.5 ) * 100;
  int angle = map(angleRead, 0, 1023, 0, 180);
  int luminosity = map(luminosityRead, 0, 1023, 0, 255);

  // Update LEDs
  setTemperatureLED(temperature);
  setAngleLED(angle);
  setLuminosityLED(luminosity);
}

void loop() {
}