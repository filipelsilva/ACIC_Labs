# Project Notes

* Ignore the top half of the intersection
* Period is 20 seconds (10 green, 10 red; with the last one second being yellow)
* For the synchronization: first send our time to west and east, then the rest
  of the algorithm
* `send` function: might need to distinguish between I2C and in-Arduino
  communication (when Arduino controls two junctions)
* Start with one junction per Arduino, then move to two

# Questions

[ ] need to detect failure of the red light -> use analogRead ?

Go go ChatGPT:
"Detecting if an LED has failed with an Arduino involves monitoring the
electrical characteristics of the LED and checking for any anomalies. Here's a
simple example using a basic method to detect LED failure. Keep in mind that
this method assumes you have a resistor connected in series with the LED and
that the LED fails in a way that affects its forward voltage or current.

```cpp
const int ledPin = 13;  // Pin connected to the LED
const int analogPin = A0;  // Analog pin connected to the junction of the LED and resistor

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Turn on the LED
  digitalWrite(ledPin, HIGH);

  // Read the analog value at the LED-resistor junction
  int sensorValue = analogRead(analogPin);

  // Calculate the voltage based on the analog value and the resolution (1023 for 10-bit ADC)
  float voltage = (sensorValue / 1023.0) * 5.0;

  // Check if the voltage is below a certain threshold (indicating possible LED failure)
  if (voltage < 4.0) {
    Serial.println("LED failure detected!");
    // Add any additional actions you want to take upon detecting LED failure
  }

  // Turn off the LED
  digitalWrite(ledPin, LOW);

  // Wait for a moment before repeating
  delay(1000);
}
```

In this example, the program turns on the LED, reads the analog value at the
junction of the LED and resistor, and converts it to a voltage. If the voltage
is below a certain threshold (4.0 volts in this case), it prints a message
indicating LED failure.

Please note that this is a simplistic approach and may not cover all failure
scenarios. For a more comprehensive LED monitoring system, you might need
additional hardware, such as a current sensor or a photodiode, depending on the
specific failure modes you want to detect. Additionally, you may want to
implement a more sophisticated algorithm or use external monitoring ICs for
better accuracy."

[ ] how to standardize the I2C communication ?
