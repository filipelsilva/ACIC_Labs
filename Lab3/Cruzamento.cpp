#include "Cruzamento.h"

Cruzamento::Cruzamento(int p_id, int p_lwr, int p_ly, int p_lsr, int p_lros,
                       int p_lrow, int p_bs, int p_bw)
    : id(p_id),
      led_w_red(p_lwr),
      led_yellow(p_ly),
      led_s_red(p_lsr),
      led_red_out_s(p_lros),
      led_red_out_w(p_lrow),
      button_s(p_bs),
      button_w(p_bw) {
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

  dutyCycleW = map(dutyCycleRounded, 0, 100, 0,
                   PERIOD_MODES_MS - 2 * LENGTH_YELLOW_MODES_MS);
  dutyCycleS = map(100 - dutyCycleRounded, 0, 100, 0,
                   PERIOD_MODES_MS - 2 * LENGTH_YELLOW_MODES_MS);
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

void Cruzamento::resetClock(unsigned int clock) {
  if (clock >= NUM_CLOCKS) {
    return;
  }

  previousTime[clock] = millis();
}

void Cruzamento::toggleYellowLED(int interval) {
  if (hasIntervalPassed(interval, 1)) {
    digitalWrite(led_yellow, !digitalRead(led_yellow));
  }
}

bool Cruzamento::check_red_led() {
  return digitalRead(led_red_out_s) != digitalRead(led_s_red) ||
         digitalRead(led_red_out_w) != digitalRead(led_w_red);
}

void Cruzamento::malfunction() {
  if (hasIntervalPassed(SANE_LOG_TIME, 5)) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer),
             "Malfunction detected | s: %d - w: %d | s: %d - w: %d",
             digitalRead(led_s_red), digitalRead(led_w_red),
             digitalRead(led_red_out_s), digitalRead(led_red_out_w));
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
    
    if (mode == 2) {
      car_ts = millis() - clock_offset;

      if (id != NUMBER_OF_INTERCEPTIONS - 1) {
        sendMessage(id + 1, Event::CAR, car_ts);
      }
      
      if (car_ts - car_ts_from_west < SPEEDING_LIMIT_TIME_MS) {
        // we have a lightning mcqueen
        log("Lightning McQueen detected");
        currentMode = 3;
        previousTime[2] = millis();
      }
    }
  }
}

void Cruzamento::check_button_press() {
  if (digitalRead(button_s) == HIGH && digitalRead(button_s) != last_value_button_s) {
    int_button_s();
  }

  if (digitalRead(button_w) == HIGH && digitalRead(button_w) != last_value_button_w) {
    int_button_w();
  }
  
  last_value_button_s = digitalRead(button_s);
  last_value_button_w = digitalRead(button_w);
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

void Cruzamento::checkClockPhase() {
  bool checked = false; 
  
  if (id != 0) {
    sendMessage(id - 1, Event::CLOCK, my_new_clock);
  }
  
  if (id != NUMBER_OF_INTERCEPTIONS - 1) {
    sendMessage(id + 1, Event::CLOCK, my_new_clock);
  }

  if (id == 0 && east_clock != -1) {
    clock = my_new_clock;
    my_new_clock = (east_clock + clock) / 2;
    checked = true;
  } else if (id == NUMBER_OF_INTERCEPTIONS - 1 && west_clock != -1) {
    clock = my_new_clock;
    my_new_clock = (west_clock + clock) / 2;
    checked = true;
  } else if (west_clock != -1 && east_clock != -1) {
    clock = my_new_clock;
    my_new_clock = (west_clock + east_clock + clock) / 3;
    checked = true;
  }
  
  if (checked && (my_new_clock - clock < ERROR_CLOCK_MS || clock - my_new_clock < ERROR_CLOCK_MS)) {
    clock_syncd = true;
    sendMessage(0, Event::SYNC, 1);
    clock_offset = first_clock - my_new_clock;
  }
}

bool Cruzamento::boot() {
  if (!clock_syncd) {
    checkClockPhase();
  }

  if (id == 0 && booting) {
    broadcastMessage(Event::MODE, mode);

    if (counter_sync == NUMBER_OF_INTERCEPTIONS - 1) {
      log("Booting done, broadcasting mode");
      broadcastMessage(Event::SYNC, 2); // only send broadcast once
      booting = false;
    }
  }

  toggleYellowLED(PERIOD_BOOT_MS);
  return hasIntervalPassed(LENGTH_BOOT_MS, 0) && !booting;
}

// normal cycle mode
void Cruzamento::mode0() {
  switch (step) {
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
void Cruzamento::mode1and2() {
  switch (step) {
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

void Cruzamento::sendMessage(int destination, Event event, uint32_t data) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
           "Sending message to %d with event %d and data %d", destination,
           event, data);
  log(buffer);
  
  if (destination == ((id % 2) == 0 ? id + 1 : id - 1)) {
    other_c->handleEvent(id, event, data);
  } else {
    Wire.beginTransmission((destination >> 1) + 1);
    Wire.write(destination);
    Wire.write(id);
    Wire.write(event);
    
    if (event == Event::CAR || event == Event::CLOCK) {
      for (int i = 0; i < 4; i++) {
        Wire.write((data >> (i * 8)) & 0xFF);
      }
    } else {
      Wire.write(data);
    }

    Wire.endTransmission();
  }
}

void Cruzamento::broadcastMessage(Event event, uint32_t data) {
  for (int i = 0; i < NUMBER_OF_INTERCEPTIONS; i++) {
    if (i != id) {
      sendMessage(i, event, data);
    }
  }
}

void Cruzamento::handleClock(int source, uint32_t p_clock) {
  if (source == id - 1) {
    west_clock = p_clock;
  } else if (source == id + 1) {
    east_clock = p_clock;
  }
}

void Cruzamento::handleCar(int source, uint32_t ts) {
  if (source == id) {
    return;
  }

  car_ts_from_west = ts;
}

void Cruzamento::handleMode(int source, uint32_t p_mode) {
  if (source != 0) {
    return;
  }

  if (p_mode == 0 || p_mode == 1 || p_mode == 2) {
    mode = p_mode;
  }
}

void Cruzamento::handleStatus(int source, uint32_t status) {
  if (source == id) {
    return;
  }
}

void Cruzamento::handleSync(int source, uint32_t sync) {
  if (id == 0) {
    if (sync == 1) {
      counter_sync += 1;
    }
  } else if (source == 0 && sync == 2) {
    log("Booting done");
    booting = false;
  }
}

void Cruzamento::handleEvent(int source, Event event, uint32_t data) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
           "Handling event from %d with event %d and data %d", source, event,
           data);
  log(buffer);

  switch (event) {
    case CLOCK:
      handleClock(source, data);
      break;
    case CAR:
      handleCar(source, data);
      break;
    case MODE:
      handleMode(source, data);
      break;
    case STATUS:
      handleStatus(source, data);
      break;
    case SYNC:
      handleSync(source, data);
      break;
  }
}

void Cruzamento::setup(int p_mode, unsigned long clock, Cruzamento *other) {
  other_c = other;

  if (id == 0) {
    mode = p_mode;
  }
  
  booting = true;
  first_clock = clock;
  
  if (id != NUMBER_OF_INTERCEPTIONS - 1) {
    sendMessage(id + 1, Event::CLOCK, clock);
  }
  
  if (id != 0) {
    sendMessage(id - 1, Event::CLOCK, clock);
  }
}

void Cruzamento::loop() {
  if (!booting) {
    check_button_press();
    
    if (hasIntervalPassed(PERIOD_MODES_MS, 6)) {
      broadcastMessage(Event::STATUS, malf ? 1 : 0);
    }

    if (check_red_led()) {
      if (malfunction_timer == 0) {
        malfunction_timer = millis();
      }
      malf = true;
      malfunction();
      return;
    }

    if (malf == true) {
      malf = false;
      reset_leds();
      previousTime[2] += millis() - malfunction_timer;
      malfunction_timer = 0;
    }
  }

  bool booting_interval_passed;
  switch (currentMode) {
    case -1:
      if (boot()) {
        reset_leds();
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "Mode %d", mode);
        log(buffer);
        currentMode = mode;
      }
      break;
    case 0:
      mode0();
      break;
    case 1:
      mode1and2();
      break;
    case 2:
      mode1and2();
      break;
  }
}