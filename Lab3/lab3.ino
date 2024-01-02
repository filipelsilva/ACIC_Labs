#include "Cruzamento.h"
#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>

#define C1_LED_RED_OUT_S 8
#define C1_LED_RED_OUT_W 7
#define C1_BUTTON_S 9
#define C1_BUTTON_W 10
#define C1_LED_W_RED 11
#define C1_LED_YELLOW 12
#define C1_LED_S_RED 13

#define C2_LED_RED_OUT_S A2
#define C2_LED_RED_OUT_W A3
#define C2_BUTTON_S 2
#define C2_BUTTON_W 3
#define C2_LED_W_RED 4
#define C2_LED_YELLOW 5
#define C2_LED_S_RED 6


// FIXME
// #define ID_PIN1 A0
#define ID_PIN2 A1
// #define ID_PIN3 A2

#define MODE 2

Cruzamento *c1 = nullptr;
Cruzamento *c2 = nullptr;

void setup() {
  Serial.begin(115200);

  // FIXME
  // pinMode(ID_PIN1, INPUT);
  pinMode(ID_PIN2, INPUT);
  // pinMode(ID_PIN3, INPUT);

  // FIXME
  // int id = digitalRead(ID_PIN3) << 3 + digitalRead(ID_PIN2) << 2 + digitalRead(ID_PIN1) << 1;
  int id = digitalRead(ID_PIN2) << 1;

  Serial.print("Initializing Cruzamentos with id ");
  Serial.print(id, DEC);
  Serial.print(" and ");
  Serial.print(id + 1, DEC);
  Serial.println(".");

  c1 = new Cruzamento(id, C1_LED_W_RED, C1_LED_YELLOW, C1_LED_S_RED,
                      C1_LED_RED_OUT_S, C1_LED_RED_OUT_W, C1_BUTTON_S,
                      C1_BUTTON_W);
  c2 = new Cruzamento(id + 1, C2_LED_W_RED, C2_LED_YELLOW, C2_LED_S_RED,
                      C2_LED_RED_OUT_S, C2_LED_RED_OUT_W, C2_BUTTON_S,
                      C2_BUTTON_W);

  Wire.begin((id >> 1) + 1);  // NOTE: this is made to avoid using channel 0
  Wire.onReceive(onReceive);
  
  unsigned long clock = millis();
  
  Serial.print("Sending setup message with mode ");
  Serial.print(MODE, DEC);
  Serial.print(" and clock ");
  Serial.print(clock, DEC);
  Serial.println("");

  c1->setup(MODE, clock, c2);
  c2->setup(MODE, clock, c1);
}

void onReceive(int bytes) {
  int destination = Wire.read();
  int source = Wire.read();

  Event event = (Event)Wire.read();

  uint32_t data = 0;

  if (event == Event::CLOCK || event == Event::CAR) {
    for (int i = 0; i < 4; i++) {
      data = data | (Wire.read() << (i * 8));
    }
  } else {
    data = Wire.read();
  }
  
  if (destination == c1->id) {
    c1->handleEvent(source, event, data);
  } else if (destination == c2->id) {
    c2->handleEvent(source, event, data);
  }
}

void loop() {
  c1->loop();
  c2->loop();
}