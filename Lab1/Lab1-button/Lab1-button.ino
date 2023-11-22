#define BUTTON 2
#define LED_BLUE 3
#define LED_RED 4
#define LED_GREEN 5
#define LED_YELLOW 6

#define SLEEP 1000

int currentButtonState = LOW;
int lastButtonState = LOW;
int currentLEDState = 0;

bool isPaused = false;
unsigned long previousTime = 0; // last time the status was updated

void setup() {
  // Create serial connection
  Serial.begin(115200);
  Serial.println("Hello World!");

  // Set LED pins to OUTPUT mode
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Set button pin to INPUT mode
  pinMode(BUTTON, INPUT);
}

bool hasIntervalPassed(int interval) {
  unsigned long now = millis();
  
  if (now >= previousTime + interval) {
    previousTime = now;
    return true;
  }

  return false;
}

bool isSystemPaused() {
  // Read the button input pin
  currentButtonState = digitalRead(BUTTON);

  // Compare with previous state
  if (currentButtonState != lastButtonState) {
    if (currentButtonState == LOW) {
      // The button went from ON to OFF
      isPaused = !isPaused;
    }
    // Avoid bouncing
    delay(50);
  }

  // Update the last button state for further iterations
  lastButtonState = currentButtonState;

  return isPaused;
}

void loop() {
  if (isSystemPaused()) {
    return;
  }

  if (!hasIntervalPassed(SLEEP)) {
    return;
  }

  switch (currentLEDState) {
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

  currentLEDState = (currentLEDState + 1) % 5;
}
