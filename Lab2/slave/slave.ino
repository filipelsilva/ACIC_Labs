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
  Serial.begin(9600);
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
  Serial.print(" | msecs: ");
  Serial.print(msecs);
  Serial.print(" | millis: ");
  Serial.print(millis());
  Serial.print(" | currentAngleLEDState: ");
  Serial.print(currentAngleLEDState);
  if (hasIntervalPassed(msecs)) {
    Serial.print(" | changing state");
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
  float temperature = (((temperatureRead / 1024.0) * 5.0 ) - 0.5 ) * 100;

  Serial.print("T: ");
  Serial.print(temperature);
  Serial.print(" (");
  Serial.print(temperatureRead, HEX);
  Serial.print(")");

  int angleRead = Wire.read() << 8;
  angleRead |= Wire.read();
  int angle = map(angleRead, 0, 1023, 0, 180);

  Serial.print(" | A: ");
  Serial.print(angle);
  Serial.print(" (");
  Serial.print(angleRead, HEX);
  Serial.print(")");

  int luminosityRead = Wire.read() << 8;
  luminosityRead |= Wire.read();
  int luminosity = map(luminosityRead, 0, 1023, 255, 0);

  Serial.print(" | L: ");
  Serial.print(luminosity);
  Serial.print(" (");
  Serial.print(luminosityRead, HEX);
  Serial.print(")");

  // Update LEDs
  setTemperatureLED(temperature);
  setAngleLED(angle);
  setLuminosityLED(luminosity);

  Serial.println();
}

void loop() {
}
