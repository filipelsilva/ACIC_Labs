int currentButtonState = LOW;
int lastButtonState = LOW;
int currentLEDState = 0;

bool isPaused = false;
unsigned long previousTime = 0; // last time the status was updated
const long interval = 1000; // one second of "delay"

void setup() {
  // Create serial connection
  Serial.begin(115200);
  Serial.println("Hello World!");

  // Set LED pins to OUTPUT mode
  for (int i = 3; i < 7; i++) {
    pinMode(i, OUTPUT);
  }

  // Set button pin to INPUT mode
  pinMode(2, INPUT);
}

bool hasIntervalPassed(int inverval) {
  unsigned long now = millis();
  
  if (now >= previousTime + interval) {
    previousTime = now;
    return true;
  }

  return false;
}

bool isSystemPaused() {
  // Read the button input pin
  currentButtonState = digitalRead(2);

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

  if (!hasIntervalPassed(interval)) {
    return;
  }

  switch (currentLEDState) {
    case 0:
      digitalWrite(4, HIGH);
      break;
    case 1:
      digitalWrite(4, LOW);
      digitalWrite(5, HIGH);
      break;
    case 2:
      digitalWrite(5, LOW);
      digitalWrite(3, HIGH);
      break;
    case 3:
      digitalWrite(3, LOW);
      digitalWrite(6, HIGH);
      break;
    case 4:
      digitalWrite(6, LOW);
      break;
  }

  currentLEDState = (currentLEDState + 1) % 5;
}
