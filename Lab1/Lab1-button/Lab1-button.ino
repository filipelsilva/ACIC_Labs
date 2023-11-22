#define BUTTON 2
#define LED_BLUE 3
#define LED_RED 4
#define LED_GREEN 5
#define LED_YELLOW 6

#define SLEEP 1000
#define PAUSED true

bool status = !PAUSED;

int currentLEDState = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World!");

	pinMode(LED_YELLOW, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);

	pinMode(BUTTON, INPUT);

	attachInterrupt(digitalPinToInterrupt(BUTTON), button_handler, FALLING);
}

void button_handler() {
	// FIX bounce
	status = !status;
}

void setLEDState(int state) {
  switch (state) {
    case 0:
      digitalWrite(LED_RED, HIGH);
      break;
    case 1:
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, HIGH);
      break;
    case 2:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, HIGH);
      break;
    case 3:
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_YELLOW, HIGH);
      break;
    case 4:
      digitalWrite(LED_YELLOW, LOW);
      break;
  }
}

void loop() {
	if (status != PAUSED) {
		setLEDState(currentLEDState);
		currentLEDState = (currentLEDState + 1) % 5;
	}
	delay(SLEEP);
}
