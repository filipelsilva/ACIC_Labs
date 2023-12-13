#include <Wire.h>

#define LED_W_RED 0 // TODO
#define LED_S_RED 0 // TODO
#define LED_W_YELLOW 0 // TODO
#define LED_S_YELLOW 0 // TODO
#define LED_W_GREEN 0 // TODO
#define LED_S_GREEN 0 // TODO

#define BUTTON_CAR_PIN 0 // TODO
#define BUTTON_MODE_PIN 0 // TODO

unsigned long previousTime = 0;

bool hasIntervalPassed(int interval) {
  unsigned long now = millis();
  if (now >= previousTime + interval) {
    previousTime = now;
    return true;
  }
  return false;
}

void toggleYellowLED(int interval) {
  if (hasIntervalPassed(interval)) {
    digitalWrite(LED_W_YELLOW, !digitalRead(LED_W_YELLOW));
    digitalWrite(LED_S_YELLOW, !digitalRead(LED_S_YELLOW));
  }
}

void malfunction() {
  Serial.println("Malfunction detected");
  toggleYellowLED(1000);
}

void boot() {
  Serial.println("Booting system");
  while (millis() < 6000) {
    toggleYellowLED(1000);
  }
}

void reset_LEDS() {
  digitalWrite(LED_W_GREEN, LOW);
  digitalWrite(LED_W_YELLOW, LOW);
  digitalWrite(LED_W_RED, LOW);
  digitalWrite(LED_S_GREEN, LOW);
  digitalWrite(LED_S_YELLOW, LOW);
  digitalWrite(LED_S_RED, LOW);
}

void mode0() {
  Serial.println("Mode 0");

  unsigned long now = millis();

  if ((now - previousTime) % 20000 < 9000) {
    // West junction is green, South junction is red
    digitalWrite(LED_W_GREEN, HIGH);
    digitalWrite(LED_W_YELLOW, LOW);
    digitalWrite(LED_W_RED, LOW);
    digitalWrite(LED_S_GREEN, LOW);
    digitalWrite(LED_S_YELLOW, LOW);
    digitalWrite(LED_S_RED, HIGH);
  } else if ((now - previousTime) % 20000 < 10000) {
    // Changing the junctions
    digitalWrite(LED_W_GREEN, LOW);
    digitalWrite(LED_W_YELLOW, HIGH);
    digitalWrite(LED_W_RED, LOW);
    digitalWrite(LED_S_GREEN, LOW);
    digitalWrite(LED_S_YELLOW, HIGH);
    digitalWrite(LED_S_RED, LOW);
  } else if ((now - previousTime) % 20000 < 19000) {
    // West junction is red, South junction is green
    digitalWrite(LED_W_GREEN, LOW);
    digitalWrite(LED_W_YELLOW, LOW);
    digitalWrite(LED_W_RED, HIGH);
    digitalWrite(LED_S_GREEN, HIGH);
    digitalWrite(LED_S_YELLOW, LOW);
    digitalWrite(LED_S_RED, LOW);
  } else {
    // Changing the junctions
    digitalWrite(LED_W_GREEN, LOW);
    digitalWrite(LED_W_YELLOW, HIGH);
    digitalWrite(LED_W_RED, LOW);
    digitalWrite(LED_S_GREEN, LOW);
    digitalWrite(LED_S_YELLOW, HIGH);
    digitalWrite(LED_S_RED, LOW);
  }

  previousTime = now;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  boot();
}

void loop() {
}
