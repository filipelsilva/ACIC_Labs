#include <Wire.h>

#define POTENTIOMETERPIN A3
#define LED_RED 4
#define LED_YELLOW 3

void setup() {
  Serial.begin(115200);
  Wire.begin(8);
  Wire.onReceive(updateLEDs);
  Wire.onRequest(requestEvent);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
}

void requestEvent() {
  int angleRead = analogRead(POTENTIOMETERPIN);
  Wire.write(angleRead >> 8);
  Wire.write(angleRead & 255);
}

void setTemperatureLED(float temperature) {
  if (temperature > 19) {
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_RED, LOW);
  }
}

void setLuminosityLED(int luminosity) {
  analogWrite(LED_YELLOW, luminosity);
}

void updateLEDs(int byteCount) {
  if (byteCount != 4) {
    Serial.println("Not the expected number of bytes");
    return;
  }

  // Read the values and map them
  int temperatureRead = Wire.read() << 8;
  temperatureRead |= Wire.read();

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
  int luminosity = map(luminosityRead, 0, 1023, 0, 255);

  // Update LEDs
  setTemperatureLED(temperature);
  setLuminosityLED(luminosity);

  Serial.print("T: ");
  Serial.print(temperature);
  Serial.print(" | L: ");
  Serial.print(luminosityRead);
  Serial.println();
}

void loop() {
}
