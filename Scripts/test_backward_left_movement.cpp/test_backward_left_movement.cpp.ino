// Define wiring pins for Motor 1
#define leftFront_enB 11
#define leftFront_in4 2
#define leftFront_in3 3
#define leftBack_enA 13
#define leftBack_in1 5
#define leftBack_in2 4

// Define wiring pins for Motor 2
#define rightFront_enA 12
#define rightFront_in1 6
#define rightFront_in2 7
#define rightBack_enB 10
#define rightBack_in4 9
#define rightBack_in3 8

// setup() method is called once when the program begins
void setup() {
  // Configure Motor 1 pins as outputs
  pinMode(leftFront_enB, OUTPUT);
  pinMode(leftFront_in4, OUTPUT);
  pinMode(leftFront_in3, OUTPUT);
  pinMode(leftBack_enA, OUTPUT);
  pinMode(leftBack_in1, OUTPUT);
  pinMode(leftBack_in2, OUTPUT);

  // Configure Motor 2 pins as outputs
  pinMode(rightFront_enA, OUTPUT);
  pinMode(rightFront_in1, OUTPUT);
  pinMode(rightFront_in2, OUTPUT);
  pinMode(rightBack_enB, OUTPUT);
  pinMode(rightBack_in4, OUTPUT);
  pinMode(rightBack_in3, OUTPUT);
}

// loop() method is called repeatedly as long as the program is running
void loop() {
  testMovement(&goBackwardLeft, 4000);
}

// Right front and left back wheels move backward **NOT TESTED
void goBackwardLeft() {
  digitalWrite(rightFront_in1, HIGH);
  digitalWrite(rightFront_in2, LOW);
  digitalWrite(leftBack_in1, HIGH);
  digitalWrite(leftBack_in2, LOW);

  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 0);
}

// Set all voltages to zero, stopping the rover **NOT TESTED
void stop() {
  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, 0);
}

// Test any kind of movement function for a specified time **NOT TESTED
// In theory, takes two parameters: a pointer to a function (e.g. to goForward()), and an amount of time in milliseconds, then performs that movement for the time given
void testMovement(void (*move)(), int time) {
  move();
  delay(time);
  stop();
}