#ifndef Cruzamento_h
#define Cruzamento_h

#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include "MessageQueue.h"

#define BUTTON_BOUNCE_MS 200

#define LENGTH_BOOT_MS 6000
#define PERIOD_BOOT_MS 1000
#define PERIOD_MODES_MS 20000
#define LENGTH_YELLOW_MODES_MS 1000

#define SANE_LOG_TIME 2000

#define NUM_CLOCKS 10

#define NUMBER_OF_INTERCEPTIONS 4
#define ERROR_CLOCK_MS 100

class Cruzamento {
 public:
  int id;
  // constructor takes the pins
  Cruzamento(int p_id, int p_led_w_red, int p_led_yellow, int p_led_s_red,
             int p_led_red_out_s, int p_led_red_out_w, int p_button_s,
             int p_button_w);
  void loop();
  void handleEvent(int source, Event event, uint32_t data);
  void setup(int mode, unsigned long clock);

 private:
  int counter_sync = 0;
  int led_w_red, led_yellow, led_s_red;
  int led_red_out_s, led_red_out_w;
  int button_s, button_w;
  int dutyCycleW, dutyCycleS;
  int carsS, carsW;
  int last_button_press_s, last_button_press_w;
  int currentMode = -1, mode;
  int step = 0;
  bool malf = false;

  unsigned long previousTime[10];
  unsigned long malfunction_timer;

  unsigned long west_clock, east_clock, clock, new_clock = millis();
  bool booting = true;
  
  unsigned int clock_offset = 0;
  unsigned long first_clock = 0;
  
  MessageQueue messageQueue = MessageQueue();
  void processMessage();
  
  // unsigned long cars[16][1000];
  // unsigned int cars_idx[16];

  void log(String msg);

  void update_duty_cycle();
  bool hasIntervalPassed(int interval, unsigned int clock);
  void resetClock(unsigned int clock);
  void toggleYellowLED(int interval);

  void malfunction();
  bool check_red_led();
  void reset_leds();
  void check_button_press();

  void int_button_s();
  void int_button_w();

  void sendMessage(int destination, Event event, uint32_t data);
  void broadcastMessage(Event event, uint32_t data);
  void handleClock(int source, uint32_t clock);
  void handleCar(int source, uint32_t ts);
  void handleMode(int source, uint32_t mode);
  void handleStatus(int source, uint32_t status);
  void handleSync(int source, uint32_t status);

  void step0();
  void step1();
  void step2();
  void step3();

  bool boot();
  void mode0();
  void mode1();
  void mode2();
};

#endif