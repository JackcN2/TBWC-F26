//Included libraries
#include <Servo.h>

//Pin definitions 
const int trigPin = 11;
const int echoPin = 10;
const int servoPin = 12;
const int motorFWD = 4;
const int motorREV = 5;
const int motorSpeed = 3;
const int light1 = A0;
const int light2 = A1;

//Global variables
const bool debug = 1; //Set to 0 when running in the real demonstration to save processing power 
const int trial = 1; //Set this before the challenge to 1 or 2 (ramp challenge or dropping challenge)
int trialDone = 0;
float duration, distance;
Servo dropper;


float currentTarget;
float distanceToleranceMeters = 0;
float distanceOffsetMeters = 0.1;

int payloadDropped = 0;
int slowZoneReached = 0;
const float trialOneForwardTargetMeters = 0.1;


const float trialTwoForwardTargetMeters = 1; // Preslowzone
const float targetInSlowZone = 0.2;

bool newDistanceSample = false;
unsigned long sampleTimeMs = 0;



const unsigned long START_DELAY_MS = 2000; //2 seconds
const unsigned long RAMP_DELAY_MS = 3000; //3 seconds
const unsigned long DROP_WAIT = 2000; //2 seconds

//init pins for input and output
void setup() {
  switch (trial) {
  case 1:
    currentTarget = trialOneForwardTargetMeters;

    break;

  case 2:
    currentTarget = trialTwoForwardTargetMeters;

    break;
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorFWD, OUTPUT);
  pinMode(motorREV, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  dropper.attach(servoPin);
  reset_dropper();
  //Start the serial monitor in debug mode 
  if (debug == 1) {
    Serial.begin(115200);
  }
  delay(START_DELAY_MS);

  if (trial == 1) {
    motor_forward();
    motor_speed(100);
    delay(RAMP_DELAY_MS / 2);
    motor_speed(150);
    delay(RAMP_DELAY_MS / 2);
  }
  if (trial == 2) {
    motor_forward();
    motor_speed(150);
  }

}

void loop() {

  sense_distance();
  if (debug == 1) {
    Serial.println(distance);
  }

  if (trialDone == 0) {

    if (trial == 1) {

      if (distance < currentTarget + distanceToleranceMeters + distanceOffsetMeters) {
        motor_reverse();
        motor_speed(150);
        delay(2500);
        motor_speed(100);
        delay(2000);

        motor_brake();
        trialDone = 1;

      }
    }
    if (trial == 2) {
      if (payloadDropped == 0) {
        if (slowZoneReached == 0) {
          if (distance < currentTarget + distanceToleranceMeters + distanceOffsetMeters) {
            motor_speed(80);
            slowZoneReached = 1;
            currentTarget = targetInSlowZone;
          }
        }
        if (slowZoneReached == 1) {
          if (distance < currentTarget + distanceToleranceMeters + distanceOffsetMeters) {
            motor_reverse();
            motor_speed(255);
            delay(2000);
            motor_brake();
            drop_payload();
            payloadDropped = 1;

          }
        }
      }
      if (payloadDropped == 1) {
        motor_reverse();
        motor_speed(80);
        delay(2500);
        motor_speed(150);
        delay(2000);

        motor_brake();
        trialDone = 1;
      }
    }

  }
}