#include <HCSR04.h>

#define trigPin 9
#define echoPin 8

// Ultrasonic sensor
long duration, cm, inches;

void setup() {
  // Serial port begin
  Serial.begin(9600);

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

void loop() {
  // Send 10 microsecond signal
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  cm = (duration / 2) / 29.1;
  inches = (duration / 2) / 74.0;

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm\n");

  delay(250);
}