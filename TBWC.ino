#include <Servo.h>


const int trigPin = 11;
const int echoPin = 10;
const int servoPin = 12;
const int motorFWD = 4;
const int motorREV = 6;
const int motorSpeed = 3;
const int light1 = 5;



const bool debug = 0;  
const int trial = 2;  


const float trialOneForwardTargetMeters = 0.60;
const float trialTwoForwardTargetMeters = 1.2;  
const float targetInSlowZone = 0.22;            
const float distanceToleranceMeters = 0;
const float distanceOffsetMeters = 0.1;        
const float returnToleranceMeters = 0.05;      
const float maxStartDistanceMeters = 4.0;     
const int CONFIRM_COUNT = 2;

//Timing
const unsigned long START_DELAY_MS = 1000;
const unsigned long RAMP_DELAY_MS = 7000;
const unsigned long DROP_WAIT = 2000;
const unsigned long SETTLE_MS = 500;           
const unsigned long SENSOR_WAIT_MS = 1500;     
const unsigned long RETURN_TIMEOUT_MS = 8000; 


float distance = 0;           
bool newDistanceSample = false; 
bool distanceReady = false;   
int belowCount = 0;
float startDistance = 0;
bool haveStartDistance = false;
Servo dropper;

enum State { DRIVING, SLOW_ZONE, DONE };
State state = DRIVING;


bool belowThreshold(float limit) {
  if (!distanceReady) return false;
  if (newDistanceSample) {
    newDistanceSample = false;
    belowCount = (distance < limit) ? belowCount + 1 : 0;
  }
  return belowCount >= CONFIRM_COUNT;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorFWD, OUTPUT);
  pinMode(motorREV, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(light1, OUTPUT);

  dropper.attach(servoPin);
  reset_dropper();

  if (debug == 1) {
    Serial.begin(115200);
  }
  delay(START_DELAY_MS);

 
  unsigned long t0 = millis();
  while (!distanceReady && millis() - t0 < SENSOR_WAIT_MS) {
    sense_distance();
  }

  newDistanceSample = false;
  belowCount = 0;

  if (trial == 1) {
    analogWrite(light1, 190);
    motor_forward();
    motor_speed(255);
    delay(RAMP_DELAY_MS / 2);
    motor_speed(255);
    delay(1000);
    analogWrite(light1, 0);
  }
  if (trial == 2) {
    analogWrite(light1, 190);
    motor_forward();
    motor_speed(255);
    delay(4000);
    analogWrite(light1, 0);
  }
}

void loop() {
  sense_distance();

  if (state == DONE) return;

  if (trial == 1) run_trial_one();
  if (trial == 2) run_trial_two();
}


void run_trial_one() {
  if (belowThreshold(trialOneForwardTargetMeters + distanceToleranceMeters + distanceOffsetMeters)) {
    analogWrite(light1, 190);
    motor_reverse();
    motor_speed(255);
    delay(7000);
    analogWrite(light1, 0);


    motor_brake();
    state = DONE;
  }
}


void run_trial_two() {
  if (state == DRIVING) {
    if (belowThreshold(trialTwoForwardTargetMeters + distanceToleranceMeters + distanceOffsetMeters)) {
      motor_speed(235);
      state = SLOW_ZONE;
      if (debug == 1) { Serial.println("In Slowzone");}
      belowCount = 0; 
    }
  } else if (state == SLOW_ZONE) {
    if (belowThreshold(targetInSlowZone + distanceToleranceMeters + distanceOffsetMeters)) {
      motor_brake();
      delay(SETTLE_MS);
      drop_payload();
      analogWrite(light1, 190);
      motor_reverse();
      motor_speed(225);
      delay(1000);
      motor_speed(255);
      delay(7500);
      motor_brake();
      analogWrite(light1, 0);
      state = DONE;
    }
  }
}

