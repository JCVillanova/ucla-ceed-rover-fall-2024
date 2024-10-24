// Define wiring pins for Motor 1
#define leftFront_enB 2
#define leftFront_in4 3
#define leftFront_in3 4
#define leftBack_enA 5
#define leftBack_in1 6
#define leftBack_in2 7

// Define wiring pins for Motor 2
#define rightFront_enA 8
#define rightFront_in1 9
#define rightFront_in2 10
#define rightBack_enB 11
#define rightBack_in4 12
#define rightBack_in3 13

// Ultrasonic sensor
#define sonar1 22
#define sonar2 23
#define sonar3 24
#define sonar4 25
#define sonar5 26
#define sonar6 27
#define MAX_DISTANCE 200
#define NUM_SONAR 6

NewPing sonar[NUM_SONAR] = { // array of ultrasonic sensors
  NewPing(sonar1, sonar1, MAX_DISTANCE),
  NewPing(sonar2, sonar2, MAX_DISTANCE),
  NewPing(sonar3, sonar3, MAX_DISTANCE),
  NewPing(sonar4, sonar4, MAX_DISTANCE),
  NewPing(sonar5, sonar5, MAX_DISTANCE),
  NewPing(sonar6, sonar6, MAX_DISTANCE)
};

int distance[NUM_SONAR]; // distance array for each sensor

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
  testMovement(&goForward, 4000);
}

// Update distance array for all sensors
void updateSonar() {
  for(int i = 0; i < NUM_SONAR; ++i) {
    distance[i] = sonar[i].ping_in();
    if(distance[i] == 0) distance[i] = MAX_DISTANCE;
  }
}

// All four wheels move forward
void goForward() {
  // Set alternating voltages on each wheel's pin
  digitalWrite(leftFront_in4, LOW);
  digitalWrite(leftFront_in3, HIGH);
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);
  digitalWrite(rightBack_in4, LOW);
  digitalWrite(rightBack_in3, HIGH);

  // Apply voltage to pins at previously specified voltages
  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 255);
}

// All four wheels move backward
void goBackward() {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightFront_in1, HIGH);
  digitalWrite(rightFront_in2, LOW);
  digitalWrite(leftBack_in1, HIGH);
  digitalWrite(leftBack_in2, LOW);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 255);
}

// Left front and right back wheels move forward, right front and left back wheels move backward
void goRight() {
  digitalWrite(leftFront_in4, LOW);
  digitalWrite(leftFront_in3, HIGH);
  digitalWrite(rightFront_in1, HIGH);
  digitalWrite(rightFront_in2, LOW);
  digitalWrite(leftBack_in1, HIGH);
  digitalWrite(leftBack_in2, LOW);
  digitalWrite(rightBack_in4, LOW);
  digitalWrite(rightBack_in3, HIGH);

  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 255);
}

// Right front and left back wheels move forward, left front and right back wheels move backward **NOT TESTED
void goLeft() {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 255);
}

// Right front and left back wheels move forward **NOT TESTED
void goForwardRight() {
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);

  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, 255);
  analogWrite(leftBack_enA, 255);
  analogWrite(rightBack_enB, 0);
}

// Left front and right back wheels move forward **NOT TESTED
void goForwardLeft() {
  digitalWrite(leftFront_in4, LOW);
  digitalWrite(leftFront_in3, HIGH);
  digitalWrite(rightBack_in4, LOW);
  digitalWrite(rightBack_in3, HIGH);

  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, 255);
}

// Left front and right back wheels move backward **NOT TESTED
void goBackwardRight() {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, 255);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, 255);
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