#include "Cruzamento.h"
#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>

#define C1_LED_RED_OUT_S A2
#define C1_LED_RED_OUT_W A3
#define C1_BUTTON_S 2
#define C1_BUTTON_W 3
#define C1_LED_W_RED 4
#define C1_LED_YELLOW 5
#define C1_LED_S_RED 6

#define C2_LED_RED_OUT_S 8
#define C2_LED_RED_OUT_W 7
#define C2_BUTTON_S 9
#define C2_BUTTON_W 10
#define C2_LED_W_RED 11
#define C2_LED_YELLOW 12
#define C2_LED_S_RED 13

// #define ID_PIN1 A0
#define ID_PIN2 A1
// #define ID_PIN3 A2 // FIXME

#define MODE 0

Cruzamento *c1 = nullptr;
Cruzamento *c2 = nullptr;

void setup() {
  Serial.begin(115200);

  // FIXME ?
  // pinMode(ID_PIN1, INPUT);
  pinMode(ID_PIN2, INPUT);

  // int id = digitalRead(ID_PIN2) << 2 + digitalRead(ID_PIN1) << 1;  // FIXME
  
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

  Wire.begin((id >> 1) + 1);  // FIXME
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

void log_message(int destination, int source, int event, uint32_t data) {
  Serial.print("Received message to ");
  Serial.print(destination, DEC);
  Serial.print(" from ");
  Serial.print(source, DEC);
  Serial.print(" with event ");
  Serial.print(event, DEC);
  Serial.print(" and data ");
  Serial.print(data, DEC);
  Serial.println("");
  Serial.flush();
}

void onReceive(int bytes) {
  // Serial.print("Received message with ");
  // Serial.print(bytes, DEC);
  // Serial.println(" bytes");

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
  
  // log_message(destination, source, event, data);

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