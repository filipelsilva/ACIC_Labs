#include <Wire.h>

#define C1_LED_RED_OUT_S A4
#define C1_LED_RED_OUT_W A5
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

Cruzamento *c1 = nullptr;
Cruzamento *c2 = nullptr;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  c1 = new Cruzamento(0, C1_LED_W_RED, C1_LED_YELLOW, C1_LED_S_RED, C1_LED_RED_OUT_S, C1_LED_RED_OUT_W, C1_BUTTON_S, C1_BUTTON_W);
  c2 = new Cruzamento(1, C2_LED_W_RED, C2_LED_YELLOW, C2_LED_S_RED, C2_LED_RED_OUT_S, C2_LED_RED_OUT_W, C2_BUTTON_S, C2_BUTTON_W);
}

void loop() {
  c1->loop();
  c2->loop();
}