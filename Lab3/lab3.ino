#include <Wire.h>

#define LED_W_RED 0 // TODO
#define LED_S_RED 0 // TODO
#define LED_W_YELLOW 0 // TODO
#define LED_S_YELLOW 0 // TODO
#define LED_W_GREEN 0 // TODO
#define LED_S_GREEN 0 // TODO

#define BUTTON_CAR_PIN 0 // TODO
#define BUTTON_MODE_PIN 0 // TODO

#define LENGTH_BOOT_MS 6000
#define PERIOD_BOOT_MS 1000
#define PERIOD_MODES_MS 20000
#define LENGTH_YELLOW_MODES_MS 1000

unsigned long previousTime = 0;

int carsS = 0;
int carsW = 0;

int dutyCycleW() {
  float dutyCycle = ((float)carsW / (float)(carsW + carsS)) * 100;

  int dutyCycleRounded = round(dutyCycle);
  if (dutyCycleRounded < 25) {
    dutyCycleRounded = 25;
  } else if (dutyCycleRounded > 75) {
    dutyCycleRounded = 75;
  }

  return map(dutyCycleRounded, 0, 100, 0, PERIOD_MODES_MS);
}

int dutyCycleS() {
  return PERIOD_MODES_MS - dutyCycleW();
}

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
  toggleYellowLED(PERIOD_BOOT_MS);
}

void boot() {
  Serial.println("Booting system");
  while (millis() < LENGTH_BOOT_MS) {
    toggleYellowLED(PERIOD_BOOT_MS);
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

  if ((now - previousTime) % PERIOD_MODES_MS < PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS) {
    // West junction is green, South junction is red
    digitalWrite(LED_W_GREEN, HIGH);
    digitalWrite(LED_W_YELLOW, LOW);
    digitalWrite(LED_W_RED, LOW);
    digitalWrite(LED_S_GREEN, LOW);
    digitalWrite(LED_S_YELLOW, LOW);
    digitalWrite(LED_S_RED, HIGH);
  } else if ((now - previousTime) % PERIOD_MODES_MS < PERIOD_MODES_MS / 2) {
    // Changing the junctions
    digitalWrite(LED_W_GREEN, LOW);
    digitalWrite(LED_W_YELLOW, HIGH);
    digitalWrite(LED_W_RED, LOW);
    digitalWrite(LED_S_GREEN, LOW);
    digitalWrite(LED_S_YELLOW, HIGH);
    digitalWrite(LED_S_RED, LOW);
  } else if ((now - previousTime) % PERIOD_MODES_MS < PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS) {
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
