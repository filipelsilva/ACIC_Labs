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
  
  for (int i = 0; i < NUMBER_OF_CARS; i++) {
    car_ts_from_west[i] = 0;
    car_ts_from_south[i] = 0;
    car_ts[i] = 0;
  }

  reset_leds();
}

void Cruzamento::log(String msg) {
  Serial.print("Cruzamento ");
  Serial.print(id, HEX);
  Serial.print(": ");
  Serial.println(msg);
}

// calculates duty cycle for each street
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

/*
 * Utility function to measure time between events
 * The clock parameter is used to support nested functions calling this function
 * Each function must possess a unique clock parameter
*/
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

// blinks yellow leds of both junctions at a given period
void Cruzamento::toggleYellowLED(int interval) {
  if (hasIntervalPassed(interval, 1)) {
    digitalWrite(led_yellow, !digitalRead(led_yellow));
  }
}

// checks if the red leds are 
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

// turn all leds off
void Cruzamento::reset_leds() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_w_red, LOW);
  digitalWrite(led_s_red, LOW);
}

// south button handler
void Cruzamento::int_button_s() {
  if (hasIntervalPassed(BUTTON_BOUNCE_MS, 3)) {
    log("Button S pressed");
    carsS += 1;
    last_button_press_s = millis();
    
    // stores the timestamp of the car that just passed in the south street
    // and communicates it to the west junction
    car_ts_from_south[car_ts_from_south_index % NUMBER_OF_CARS] = millis() - clock_offset;
    if (id != NUMBER_OF_INTERCEPTIONS - 1) {
      sendMessage(id + 1, Event::CAR, car_ts_from_south[car_ts_from_south_index % NUMBER_OF_CARS]);
    }
    car_ts_from_south_index++;
  }
}

// west button handler
void Cruzamento::int_button_w() {
  if (hasIntervalPassed(BUTTON_BOUNCE_MS, 4)) {
    log("Button W pressed");
    carsW += 1;
    last_button_press_w = millis();
    
    if (mode == 2) {
      car_ts[car_ts_index % NUMBER_OF_CARS] = millis() - clock_offset;

      if (id != NUMBER_OF_INTERCEPTIONS - 1) {
        sendMessage(id + 1, Event::CAR, car_ts[car_ts_index % NUMBER_OF_CARS]);
      }
      
      if (car_ts[car_ts_index % NUMBER_OF_CARS] - car_ts_from_west[(car_ts_from_west_index - 1) % NUMBER_OF_CARS] < SPEEDING_LIMIT_TIME_MS 
              && id > 0) {
        // we have a speeder
        log("Lightning McQueen detected");

        if (step != 0) { // only change light if its not red already
          step = 3;
        }

        previousTime[2] = millis();
      }
      
      car_ts_index++;
    }
  }
}

// polls for button presses
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

// clock sync procedure
void Cruzamento::checkClockPhase() {
  bool checked = false; 
  
  if (id != 0) {
    sendMessage(id - 1, Event::CLOCK, my_new_clock);
  }
  
  if (id != NUMBER_OF_INTERCEPTIONS - 1) {
    sendMessage(id + 1, Event::CLOCK, my_new_clock);
  }

  if (id == 0 && east_clock != -1) { // has no west
    clock = my_new_clock;
    my_new_clock = (east_clock + clock) / 2;
    checked = true;
  } else if (id == NUMBER_OF_INTERCEPTIONS - 1 && west_clock != -1) { // has no east
    clock = my_new_clock;
    my_new_clock = (west_clock + clock) / 2;
    checked = true;
  } else if (west_clock != -1 && east_clock != -1) {
    clock = my_new_clock;
    my_new_clock = (west_clock + east_clock + clock) / 3;
    checked = true;
  }
  
  // need to check for absolute value
  if (checked && (my_new_clock - clock < ERROR_CLOCK_MS || clock - my_new_clock < ERROR_CLOCK_MS)) {
    clock_syncd = true;
    sendMessage(0, Event::SYNC, 1);
    clock_offset = first_clock - my_new_clock;
  }
}

// handles the booting phase
bool Cruzamento::boot() {
  // sync clocks
  if (!clock_syncd) {
    checkClockPhase();
  }

  // broadcast mode and sync ack
  if (id == 0 && booting) {
    broadcastMessage(Event::MODE, mode);

    if (counter_sync == NUMBER_OF_INTERCEPTIONS - 1) {
      log("Booting done, broadcasting mode");
      broadcastMessage(Event::SYNC, 2);
      booting = false;
    }
  }

  // toggle leds and waits for the boot period to finish
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

// handles the communication between junctions
void Cruzamento::sendMessage(int destination, Event event, long data) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
           "Sending message to %d with event %d and data %ld", destination,
           event, data);
  log(buffer);
  
  if (destination == ((id % 2) == 0 ? id + 1 : id - 1)) {
    // messages to the same node cannot be sent through I2C
    other_c->handleEvent(id, event, data);
  } else {
    Wire.beginTransmission((destination >> 1) + 1);
    Wire.write(destination);
    Wire.write(id);
    Wire.write(event);
    
    if (event == Event::CAR || event == Event::CLOCK) {
      // send timestamp in little endian
      char d[4];
      memcpy(d, &data, 4);
      Wire.write(d, 4);
    } else {
      Wire.write(data);
    }

    Wire.endTransmission();
  }
}

// sends message to all interceptions
void Cruzamento::broadcastMessage(Event event, long data) {
  for (int i = 0; i < NUMBER_OF_INTERCEPTIONS; i++) {
    if (i != id) {
      sendMessage(i, event, data);
    }
  }
}

// clock event handler
void Cruzamento::handleClock(int source, long p_clock) {
  if (source == id - 1) {
    west_clock = p_clock;
  } else if (source == id + 1) {
    east_clock = p_clock;
  }
}

// car event handler
void Cruzamento::handleCar(int source, long ts) {
  if (source == id) {
    return;
  }

  car_ts_from_west[car_ts_from_west_index] = ts;
  car_ts_from_west_index++;
}

// mode event handler
void Cruzamento::handleMode(int source, long p_mode) {
  if (source != 0) {
    return;
  }

  if (p_mode == 0 || p_mode == 1 || p_mode == 2) {
    mode = p_mode;
  }
}

// status event handler
void Cruzamento::handleStatus(int source, long status) {
  if (source == id) {
    return;
  }
}

// sync event handler
void Cruzamento::handleSync(int source, long sync) {
  if (id == 0) {
    if (sync == 1) {
      counter_sync += 1;
    }
  } else if (source == 0 && sync == 2) {
    log("Booting done");
    booting = false;
  }
}

// forwads an event to its handler
void Cruzamento::handleEvent(int source, Event event, long data) {
  char buffer[100];
  snprintf(buffer, sizeof(buffer),
           "Handling event from %d with event %d and data %ld", source, event,
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

// sets up some properties of the junction that cannot be set in the constructor
// and sends the first clock event to its neighbors
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

// main loop
void Cruzamento::loop() {
  if (!booting) { // handles button polling and malfunction checking
    check_button_press();
    
    if (hasIntervalPassed(PERIOD_MODES_MS, 6)) {
      broadcastMessage(Event::STATUS, malf ? 1 : 0);
    }

    if (check_red_led()) {
      malf = true;
      malfunction();
      return;
    }

    if (malf == true) {
      malf = false;
      reset_leds();
    }
  }

  bool booting_interval_passed;
  switch (currentMode) {
    case -1:
      if (boot()) { // if boot is finished set the appropriate mode
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