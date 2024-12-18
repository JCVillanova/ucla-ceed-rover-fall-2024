#include <NewPing.h>
#include <Wire.h>
#include <MPU6050_light.h>
#include <Servo.h>

MPU6050 mpu(Wire);

const int SENSOR_AVERAGE_RANGE = 4;

const int speed = 255;
bool sensorsSetUp = false;
bool inSearchArea = false;
bool objectPickedUp = false;
bool leftDidNotWork = false;
bool obstacleFound = false;
bool extraRotate = false;
double sensorData[6][SENSOR_AVERAGE_RANGE]; // 2D array to store sensor data values for each of 6 sensors

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
#define sonar1 25 // front left
#define sonar2 26 // front right
#define sonar3 24 // left
#define sonar4 23 // right
#define sonar5 22 // back
#define sonar6 27 // for claw
#define MAX_DISTANCE 400
#define NUM_SONAR 6

// Servo motor
#define servoPWM 44
Servo myServo;

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
  // Serial port begin
  Serial.begin(9600);
  Serial.println("Setup started");

  Wire.begin();
  
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");

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
  if(!sensorsSetUp) {
    for(int i = 0; i < SENSOR_AVERAGE_RANGE; ++i) {
      delay(40);
      updateSonar(sensorData, i);
    }

    for(int i = 0; i < NUM_SONAR; ++i) {
      for(int j = 0; j < SENSOR_AVERAGE_RANGE; ++j) {
        distance[i] += sensorData[i][j];
      }
      distance[i] /= SENSOR_AVERAGE_RANGE;
    }
    sensorsSetUp = true;
  }

  mpu.update();

  delay(40);
  updateSonar(sensorData);

  if(distance[2] + distance[3] > 150) {
    inSearchArea = true;
  }

  if(inSearchArea && !objectPickedUp) {
    for(int i = 0; i < 4; ++i) updateSonar(sensorData);
    while(distance[3] > 90) {
      goForward();
    }
    delay(750);
    stop();
    while(mpu.getAngleZ() - 85 > 5 || mpu.getAngleZ() - 85 < -5) {
      rotateLeft(128);
      mpu.update();
    }
    /*if(!extraRotate) {
      delay(1500);
      stop();
      delay(200);
      extraRotate = true;
    }*/
    if(distance[5] >= 8) {
      goBackward();
    } else {
      stop();
      delay(200);
      updateSonar(sensorData);
      if(distance[5] <= 5) {
        myServo.attach(servoPWM);
        myServo.write(0);
        delay(1500);
        objectPickedUp = true;
        Serial.println("Object was picked up");
      }
    }
  } else if(objectPickedUp) {
    rotateLeft(128);
    delay(2250);
    
    goBackward();
    delay(8000);
    stop();
    while(1);
  } else {
      if(frontDistance() <= 30) {
        if(distance[2] >= 20 && !leftDidNotWork) {
          obstacleFound = true;
          Serial.println("Going left");
          goLeft();
          if(distance[2] <= 20) leftDidNotWork = true;
        }
        else {
          Serial.println("Going right");
          goRight();
        }
      } else {
        if(obstacleFound) {
          stop();
          delay(100);
          while(mpu.getAngleZ() > 5) {
            Serial.println("Realigning right");
            rotateRight(50);
            mpu.update();
          }
          while(mpu.getAngleZ() < -5) {
            Serial.println("Realigning left");
            rotateLeft(50);
            mpu.update();
          }
          obstacleFound = false;
        }
        leftDidNotWork = false;
        Serial.println("Going forward probably");
        goForward();
      }
  }
}

// Update distance array for all sensors
void updateSonar(double sensorData[][SENSOR_AVERAGE_RANGE]) {
  double ping;
  double sum[6] = {0, 0, 0, 0, 0, 0};
  for(int i = 0; i < NUM_SONAR; ++i) {
    ping = sonar[i].ping_cm(); // store ping value

    // Shift all values left in the array, deleting the oldest value to make room for new value
    for(int j = 0; j < SENSOR_AVERAGE_RANGE - 1; ++j) {
      sensorData[i][j] = sensorData[i][j+1];
    }
    sensorData[i][SENSOR_AVERAGE_RANGE-1] = ping;

    // Add up 16 past values and average them out
    for(int k = 0; k < SENSOR_AVERAGE_RANGE; ++k) {
      sum[i] += sensorData[i][k];
    }
    sum[i] /= SENSOR_AVERAGE_RANGE;
    distance[i] = sum[i];

    //if(distance[i] == 0) distance[i] = MAX_DISTANCE;
  }
}

void updateSonar(double sensorData[][SENSOR_AVERAGE_RANGE], int index) {
  for(int i = 0; i < NUM_SONAR; ++i) {
    sensorData[i][index] = sonar[i].ping_cm();
  }
}

void printDistances() {
  for(int i = 0; i < NUM_SONAR; ++i) {
    Serial.print("Sonar ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(distance[i]);
  }
}

bool obstacleTooCloseFront() {
  if(distance[0] <= 25 || distance[1] <= 25) return true;
  return false;
}

double frontDistance() {
  if(distance[0] < distance[1]) return distance[0];
  else return distance[1];
}

bool obstacleTooCloseLeft() {
  if(distance[2] <= 50) return true;
  return false;
}

bool obstacleTooCloseRight() {
  if(distance[3] <= 50) return true;
  return false;
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
  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
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

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
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

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
}

// Rotates the rover to the right
void rotateRight(int rotateSpeed) {
  digitalWrite(leftFront_in4, LOW);
  digitalWrite(leftFront_in3, HIGH);
  digitalWrite(rightFront_in1, HIGH);
  digitalWrite(rightFront_in2, LOW);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
}

// Right front and left back wheels move forward, left front and right back wheels move backward
void goLeft() {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
}

// Rotates the rover to the left
void rotateLeft(int rotateSpeed) {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, HIGH);
  digitalWrite(leftBack_in2, LOW);
  digitalWrite(rightBack_in4, LOW);
  digitalWrite(rightBack_in3, HIGH);

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, speed);
}

// Right front and left back wheels move forward 
void goForwardRight() {
  digitalWrite(rightFront_in1, LOW);
  digitalWrite(rightFront_in2, HIGH);
  digitalWrite(leftBack_in1, LOW);
  digitalWrite(leftBack_in2, HIGH);

  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, 0);
}

// Left front and right back wheels move forward 
void goForwardLeft() {
  digitalWrite(leftFront_in4, LOW);
  digitalWrite(leftFront_in3, HIGH);
  digitalWrite(rightBack_in4, LOW);
  digitalWrite(rightBack_in3, HIGH);

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, speed);
}

// Left front and right back wheels move backward 
void goBackwardRight() {
  digitalWrite(leftFront_in4, HIGH);
  digitalWrite(leftFront_in3, LOW);
  digitalWrite(rightBack_in4, HIGH);
  digitalWrite(rightBack_in3, LOW);

  analogWrite(leftFront_enB, speed);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, speed);
}

// Right front and left back wheels move backward 
void goBackwardLeft() {
  digitalWrite(rightFront_in1, HIGH);
  digitalWrite(rightFront_in2, LOW);
  digitalWrite(leftBack_in1, HIGH);
  digitalWrite(leftBack_in2, LOW);

  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, speed);
  analogWrite(leftBack_enA, speed);
  analogWrite(rightBack_enB, 0);
}

// Set all voltages to zero, stopping the rover 
void stop() {
  analogWrite(leftFront_enB, 0);
  analogWrite(rightFront_enA, 0);
  analogWrite(leftBack_enA, 0);
  analogWrite(rightBack_enB, 0);
}

// Test any kind of movement function for a specified time 
// Takes two parameters: a pointer to a function (e.g. to goForward()), and an amount of time in milliseconds, then performs that movement for the time given
void testMovement(void (*move)(), int time) {
  move();
  delay(time);
  stop();
}
