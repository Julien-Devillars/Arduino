// Simple example reading a push button on pin 2 and lighting an LED on pin 3

const int buttonPin = 2;
const int ledPin = 3;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // configure pin 2 as input with pull-up resistor
  pinMode(ledPin, OUTPUT);           // configure pin 3 as output
}

void loop() {
  int buttonState = digitalRead(buttonPin);  // read the state of the push button
  // When using INPUT_PULLUP, pressed button gives LOW
  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);    // turn LED on
  } else {
    digitalWrite(ledPin, LOW);     // turn LED off
  }
}
