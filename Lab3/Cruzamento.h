#include "Arduino.h"

#define BUTTON_BOUNCE_MS 200

#define LENGTH_BOOT_MS 6000
#define PERIOD_BOOT_MS 1000
#define PERIOD_MODES_MS 20000
#define LENGTH_YELLOW_MODES_MS 1000

#define NUM_CLOCKS 10

class Cruzamento {
  public:
    // constructor takes the pins
    Cruzamento(int p_id, int p_led_w_red, int p_led_yellow, int p_led_s_red, int p_led_red_out_s, int p_led_red_out_w, int p_button_s, int p_button_w);  
    void loop();
  
  private:
    int led_w_red, led_yellow, led_s_red, led_red_out_s, led_red_out_w, button_s, button_w;
    int dutyCycleW, dutyCycleS;
    int carsS, carsW;
    int last_button_press_s, last_button_press_w;
    int currentMode = -1;
    int step = 0;
    int id;
    unsigned long previousTime[10];
    
    void log(String msg);

    void update_duty_cycle();
    bool hasIntervalPassed(int interval, unsigned int clock);
    void toggleYellowLED(int interval);

    void malfunction();
    bool check_red_led();
    void reset_leds();
    int read_mode();
    void check_button_press();

    void int_button_s();
    void int_button_w();

    void step0();
    void step1();
    void step2();
    void step3();

    bool boot();
    void mode0();
    void mode1();
    void mode2();
};