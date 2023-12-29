#include "Cruzamento.h"

Cruzamento::Cruzamento(int p_id, int p_lwr, int p_ly, int p_lsr, int p_lros, int p_lrow, int p_bs, int p_bw): 
    id(p_id), led_w_red(p_lwr), led_yellow(p_ly), led_s_red(p_lsr), led_red_out_s(p_lros), led_red_out_w(p_lrow), button_s(p_bs), button_w(p_bw) {
  pinMode(led_w_red, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_s_red, OUTPUT);
  pinMode(led_red_out_s, INPUT);
  pinMode(led_red_out_w, INPUT);
  pinMode(button_s, INPUT);
  pinMode(button_w, INPUT);

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

void Cruzamento::log(String msg) {
  Serial.print("Cruzamento ");
  Serial.print(id, HEX);
  Serial.print(": ");
  Serial.println(msg);
}

void Cruzamento::update_duty_cycle() {
  log("Updating duty cycle");

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
  return digitalRead(led_red_out_s) != digitalRead(led_s_red) || digitalRead(led_red_out_w) != digitalRead(led_w_red);
}

void Cruzamento::malfunction() {
  if (hasIntervalPassed(SANE_LOG_TIME, 5)) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Malfunction detected | s: %d - w: %d | s: %d - w: %d", digitalRead(led_s_red), digitalRead(led_w_red), digitalRead(led_red_out_s), digitalRead(led_red_out_w));
    log(String(buffer));
  }
  toggleYellowLED(PERIOD_BOOT_MS);
}

void Cruzamento::reset_leds() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_w_red, LOW);
  digitalWrite(led_s_red, LOW);
}

void Cruzamento::int_button_s() {
  if (hasIntervalPassed(BUTTON_BOUNCE_MS, 3)) {
    log("Button S pressed");
    carsS += 1;
    last_button_press_s = millis();
  }
}

void Cruzamento::int_button_w() {
  if (hasIntervalPassed(BUTTON_BOUNCE_MS, 4)) {
    log("Button W pressed");
    carsW += 1;
    last_button_press_w = millis();
  }
}

int Cruzamento::read_mode() {
  return 1;
}

void Cruzamento::check_button_press() {
  if (digitalRead(button_s) == HIGH) {
    int_button_s();
  }

  if (digitalRead(button_w) == HIGH) {
    int_button_w();
  }
}

// West junction is green, South junction is red
void Cruzamento::step0() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_s_red, HIGH);
}

// Changing the junctions
void Cruzamento::step1() {
  digitalWrite(led_s_red, LOW);
  digitalWrite(led_yellow, HIGH);
}

// West junction is red, South junction is green
void Cruzamento::step2() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_w_red, HIGH);
}

// Changing the junctions
void Cruzamento::step3() {
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
      step0();
      if (hasIntervalPassed(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS, 2)) {
        step = 1;
      }
      break;
    case 1:
      step1();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 2;
      }
      break;
    case 2:
      step2();
      if (hasIntervalPassed(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS, 2)) {
        step = 3;
      }
      break;
    case 3:
      step3();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 0;
      }
      break;
  }
}


// takes into account duty cycle  
void Cruzamento::mode1() {
  switch(step) {
    case 0:
      step0();
      if (hasIntervalPassed(dutyCycleS, 2)) {
        step = 1;
      }
      break;
    case 1:
      step1();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 2;
      }
      break;
    case 2:
      step2();
      if (hasIntervalPassed(dutyCycleW, 2)) {
        step = 3;
      }
      break;
    case 3:
      step3();
      if (hasIntervalPassed(LENGTH_YELLOW_MODES_MS, 2)) {
        step = 0;
        update_duty_cycle();
      }
      break;
  }
}

void Cruzamento::mode2() {

}

void Cruzamento::loop() {
  check_button_press();
  
  if (check_red_led()) {
    malf = true;
    malfunction();
    return;
  }
  
  if (malf) {
    malf = false;
    reset_leds();
    step = 0;
    if (currentMode != 0) {
      update_duty_cycle();
    }
  }
  
  switch (currentMode) {
    case -1:
      if (boot()) {
        reset_leds();
        currentMode = read_mode();
      }
      break;
    case 0:
      mode0();
      break;
    case 1:
      mode1();
      break;
    case 2:
      mode2();
      break;
  }
}