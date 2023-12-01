#include <Wire.h>

#define LED_BLUE 2
#define LED_RED 4
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
  Serial.begin(115200);
  Wire.begin(8);
  Wire.onReceive(updateLEDs);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
}

void setTemperatureLED(float temperature) {
  if (temperature > 24) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }
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

void setLuminosityLED(int luminosity) {
  analogWrite(LED_YELLOW, luminosity);
}

void updateLEDs(int byteCount) {
  if (byteCount != 6) {
    Serial.println("Not the expected number of bytes");
    return;
  }

  // Read the values and map them
  int temperatureRead = Wire.read() << 8;
  temperatureRead |= Wire.read();

  int angleRead = Wire.read() << 8;
  angleRead |= Wire.read();

  int luminosityRead = Wire.read() << 8;
  luminosityRead |= Wire.read();

  // Serial.print("T: ");
  // Serial.print(temperatureRead, HEX);
  // Serial.print(" | A: ");
  // Serial.print(angleRead, HEX);
  // Serial.print(" | L: ");
  // Serial.print(luminosityRead, HEX);
  // Serial.println();

  // Map the values
  float temperature = (((temperatureRead / 1024.0) * 5.0 ) - 0.5 ) * 100;
  int angle = map(angleRead, 0, 1023, 0, 180);
  int luminosity = map(luminosityRead, 0, 1023, 255, 0);

  // Update LEDs
  setTemperatureLED(temperature);
  setAngleLED(angle);
  setLuminosityLED(luminosity);

  Serial.print("T: ");
  Serial.print(temperature);
  Serial.print(" | A: ");
  Serial.print(angleRead);
  Serial.print(" | L: ");
  Serial.print(luminosityRead);
  Serial.println();
}

void loop() {
}
