#ifndef Cruzamento_h
#define Cruzamento_h

#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>

#define BUTTON_BOUNCE_MS 200

#define LENGTH_BOOT_MS 6000
#define PERIOD_BOOT_MS 1000
#define PERIOD_MODES_MS 20000
#define LENGTH_YELLOW_MODES_MS 1000

#define SANE_LOG_TIME 2000

#define NUM_CLOCKS 10

#define NUMBER_OF_INTERCEPTIONS 4
#define NUMBER_OF_CARS 10
#define ERROR_CLOCK_MS 100

#define SPEEDING_LIMIT_TIME_MS 4000

enum Event { CLOCK = 0, CAR = 1, MODE = 2, STATUS = 3, SYNC = 4 };

class Cruzamento {
 public:
  int id;
  // constructor takes the pins
  Cruzamento(int p_id, int p_led_w_red, int p_led_yellow, int p_led_s_red,
             int p_led_red_out_s, int p_led_red_out_w, int p_button_s,
             int p_button_w);
  void loop();
  void handleEvent(int source, Event event, long data);
  void setup(int mode, unsigned long clock, Cruzamento* other);

 private:
  int counter_sync = 0;

  int led_w_red, led_yellow, led_s_red;
  int led_red_out_s, led_red_out_w;
  int button_s, button_w;

  int dutyCycleW, dutyCycleS;
  int carsS, carsW;

  long car_ts_from_west[NUMBER_OF_CARS];
  int car_ts_from_west_index = 0;
  long car_ts_from_south[NUMBER_OF_CARS];
  int car_ts_from_south_index = 0;
  long car_ts[NUMBER_OF_CARS];
  int car_ts_index = 0;

  int last_button_press_s, last_button_press_w;
  bool last_value_button_s, last_value_button_w;

  int currentMode = -1, mode;
  int step = 0;

  bool malf = false;

  unsigned long previousTime[10];
  unsigned long malfunction_timer;

  long west_clock, east_clock, clock, my_new_clock = millis(), clock_offset, first_clock = 0;
  bool clock_syncd = false, booting;
  
  Cruzamento *other_c;
  
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

  void sendMessage(int destination, Event event, long data);
  void broadcastMessage(Event event, long data);
  void handleClock(int source, long clock);
  void handleCar(int source, long ts);
  void handleMode(int source, long mode);
  void handleStatus(int source, long status);
  void handleSync(int source, long status);

  void step0();
  void step1();
  void step2();
  void step3();

  bool boot();
  void checkClockPhase();

  void mode0();
  void mode1and2();
};

#endif