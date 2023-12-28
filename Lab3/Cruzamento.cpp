#include "Cruzamento.h"

Cruzamento::Cruzamento(int p_id, int p_led_w_red, int p_led_yellow, int p_led_s_red, int p_led_red_out, int p_button_s, int p_button_w): id(p_id), led_w_red(p_led_w_red), led_yellow(p_led_yellow), led_s_red(p_led_s_red), button_s(p_button_s), button_w(p_button_w) {
  pinMode(p_led_w_red, OUTPUT);
  pinMode(p_led_yellow, OUTPUT);
  pinMode(p_led_s_red, OUTPUT);
  pinMode(p_led_red_out, INPUT);
  pinMode(p_button_s, INPUT);
  pinMode(p_button_w, INPUT);

  carsS = 0;
  carsW = 0;

  dutyCycleW = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;
  dutyCycleS = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;

  last_button_press_w = 0;
  last_button_press_s = 0;
  
  for (int i = 0; i < 10; i++) {
    previousTime[i] = millis();
  }
  
  reset_leds();
}

void Cruzamento::update_duty_cycle() {
  Serial.println("Updating duty cycle");

  if (carsW + carsS == 0) {
    dutyCycleW = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;
    dutyCycleS = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;
    return;
  }

  float dutyCycle = ((float)carsW / (float)(carsW + carsS)) * 100;

  int dutyCycleRounded = round(dutyCycle);
  if (dutyCycleRounded < 25) {
    dutyCycleRounded = 25;
  } else if (dutyCycleRounded > 75) {
    dutyCycleRounded = 75;
  }

  dutyCycleW = map(dutyCycleRounded, 0, 100, 0, PERIOD_MODES_MS - 2 * LENGTH_YELLOW_MODES_MS);
  dutyCycleS = map(100 - dutyCycleRounded, 0, 100, 0, PERIOD_MODES_MS - 2 * LENGTH_YELLOW_MODES_MS);
  carsW = 0;
  carsS = 0;
}


bool Cruzamento::hasIntervalPassed(int interval, unsigned int clock) {
  if (clock >= NUM_CLOCKS) {
    return false;
  }

  unsigned long now = millis();

  if (now >= previousTime[clock] + interval) {
    previousTime[clock] = now;
    return true;
  }

  return false;
}

void Cruzamento::toggleYellowLED(int interval) {
  if (hasIntervalPassed(interval, 1)) {
    digitalWrite(led_yellow, !digitalRead(led_yellow));
  }
}

bool Cruzamento::check_red_led() {
  // TODO
  return false;
}

void Cruzamento::malfunction() {
  Serial.println("Malfunction detected");
  toggleYellowLED(PERIOD_BOOT_MS);
}

void Cruzamento::reset_leds() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_w_red, LOW);
  digitalWrite(led_s_red, LOW);
}

void Cruzamento::int_button_s() {
  if (millis() - last_button_press_s > BUTTON_BOUNCE_MS) {
    Serial.println("Button S pressed");
    carsS += 1;
    last_button_press_s = millis();
  }
}

void Cruzamento::int_button_w() {
  if (millis() - last_button_press_w > BUTTON_BOUNCE_MS) {
    Serial.println("Button W pressed");
    carsW += 1;
    last_button_press_w = millis();
  }
}

// West junction is green, South junction is red
void Cruzamento::step1() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_s_red, HIGH);
}

// Changing the junctions
void Cruzamento::step2() {
  digitalWrite(led_s_red, LOW);
  digitalWrite(led_yellow, HIGH);
}

// West junction is red, South junction is green
void Cruzamento::step3() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_w_red, HIGH);
}

// Changing the junctions
void Cruzamento::step4() {
  digitalWrite(led_w_red, LOW);
  digitalWrite(led_yellow, HIGH);
}

bool Cruzamento::boot() {
  toggleYellowLED(PERIOD_BOOT_MS);
  return hasIntervalPassed(LENGTH_BOOT_MS, 0);
}

// normal cycle mode
void Cruzamento::mode0() {
  switch(step) {
    case 0:
      step1();
      if (hasIntervalPassed(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS, 2)) {
        step = 1;
      }
      break;
    case 1:
      step2();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 2;
      }
      break;
    case 2:
      step3();
      if (hasIntervalPassed(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS, 2)) {
        step = 3;
      }
      break;
    case 3:
      step4();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 0;
      }
      break;
  }
}


// takes into account duty cycle  
void Cruzamento::mode1() {
  if (check_red_led()) {
    // // // // // // // // // do_stuff
  }

  step1(); // S is red here
  delay(dutyCycleS - LENGTH_YELLOW_MODES_MS); 
  step2();
  delay(LENGTH_YELLOW_MODES_MS);
  step3(); // W is red here
  delay(dutyCycleW - LENGTH_YELLOW_MODES_MS);
  step4();
  delay(LENGTH_YELLOW_MODES_MS);
  update_duty_cycle();
}

void Cruzamento::mode2() {

}

void Cruzamento::loop() {
  switch (currentMode) {
    case 0:
      if (boot()) {
        reset_leds();
        currentMode = 1;
      }
      break;
    case 1:
      mode0();
      break;
    case 2:
      mode1();
      break;
  }
}