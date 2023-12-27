#include <Wire.h>

#define LED_W_RED 4 // TODO
#define LED_W_YELLOW 5 // TODO
#define LED_W_GREEN 6 // TODO
#define LED_S_RED 7 // TODO
#define LED_S_YELLOW 8 // TODO
#define LED_S_GREEN 9 // TODO

#define BUTTON_S 2
#define BUTTON_W 3
#define BUTTON_BOUNCE_MS 350

#define LENGTH_BOOT_MS 6000
#define PERIOD_BOOT_MS 1000
#define PERIOD_MODES_MS 20000
#define LENGTH_YELLOW_MODES_MS 1000

// Change current mode of operation
int currentMode = 0;

int changeMode() {
  Serial.print("Changing mode to ");
  Serial.println(currentMode);
  currentMode = (currentMode + 1) % 3;
}

// Calculate duty cycle
int carsS = 0;
int carsW = 0;

int dutyCycleW = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;
int dutyCycleS = PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS;

// Last button presses
int last_button_press_w = 0;
int last_button_press_s = 0;

void update_duty_cycle() {
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

// Time function
unsigned long previousTime = 0;

bool hasIntervalPassed(int interval) {
  unsigned long now = millis();
  if (now >= previousTime + interval) {
    previousTime = now;
    return true;
  }
  return false;
}

// Operating mode functions
void toggleYellowLED(int interval) {
  if (hasIntervalPassed(interval)) {
    digitalWrite(LED_W_YELLOW, !digitalRead(LED_W_YELLOW));
    digitalWrite(LED_S_YELLOW, !digitalRead(LED_S_YELLOW));
  }
}

void malfunction() {
  Serial.println("Malfunction detected");
  toggleYellowLED(PERIOD_BOOT_MS);
}

void boot() {
  reset_LEDS();
  Serial.println("Booting system");
  while (millis() < LENGTH_BOOT_MS) {
    toggleYellowLED(PERIOD_BOOT_MS);
  }
  reset_LEDS();
}

void reset_LEDS() {
  digitalWrite(LED_W_GREEN, LOW);
  digitalWrite(LED_W_YELLOW, LOW);
  digitalWrite(LED_W_RED, LOW);
  digitalWrite(LED_S_GREEN, LOW);
  digitalWrite(LED_S_YELLOW, LOW);
  digitalWrite(LED_S_RED, LOW);
}

// West junction is green, South junction is red
void step1() {
  digitalWrite(LED_W_YELLOW, LOW);
  digitalWrite(LED_S_YELLOW, LOW);
  digitalWrite(LED_W_GREEN, HIGH);
  digitalWrite(LED_S_RED, HIGH);
}

// Changing the junctions
void step2() {
  digitalWrite(LED_W_GREEN, LOW);
  digitalWrite(LED_S_RED, LOW);
  digitalWrite(LED_W_YELLOW, HIGH);
  digitalWrite(LED_S_YELLOW, HIGH);
}

// West junction is red, South junction is green
void step3() {
  digitalWrite(LED_W_YELLOW, LOW);
  digitalWrite(LED_S_YELLOW, LOW);
  digitalWrite(LED_W_RED, HIGH);
  digitalWrite(LED_S_GREEN, HIGH);
}

// Changing the junctions
void step4() {
  digitalWrite(LED_W_RED, LOW);
  digitalWrite(LED_S_GREEN, LOW);
  digitalWrite(LED_W_YELLOW, HIGH);
  digitalWrite(LED_S_YELLOW, HIGH);
}

// normal cycle mode
void mode0() {
  step1();
  delay(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS);
  step2();
  delay(LENGTH_YELLOW_MODES_MS);
  step3();
  delay(PERIOD_MODES_MS / 2 - LENGTH_YELLOW_MODES_MS);
  step4();
  delay(LENGTH_YELLOW_MODES_MS);
}

// takes into account duty cycle  
void mode1() {
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

void int_button_s() {
  if (millis() - last_button_press_s > BUTTON_BOUNCE_MS) {
    Serial.println("Button S pressed");
    carsS += 1;
    last_button_press_s = millis();
  }
}

void int_button_w() {
  if (millis() - last_button_press_w > BUTTON_BOUNCE_MS) {
    Serial.println("Button W pressed");
    carsW += 1;
    last_button_press_w = millis();
  }
}

void setup() {
  pinMode(LED_W_RED, OUTPUT);
  pinMode(LED_W_YELLOW, OUTPUT);
  pinMode(LED_W_GREEN, OUTPUT);
  pinMode(LED_S_RED, OUTPUT);
  pinMode(LED_S_YELLOW, OUTPUT);
  pinMode(LED_S_GREEN, OUTPUT);
  pinMode(BUTTON_S, INPUT);
  pinMode(BUTTON_W, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON_S), int_button_s, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_W), int_button_w, FALLING);
  Serial.begin(115200);
  Wire.begin();
  boot();
}

void loop() {
  mode1();
}
