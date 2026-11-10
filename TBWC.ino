//Included libraries
#include <Servo.h>

//Pin definitions 
const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 12;
const int motorFWD = 4;
const int motorREV = 5;
const int motorSpeed = 3;

//Global variables
const bool debug = 1; //Set to 0 when running in the real demonstration to save processing power (disables serial prints)
const int trial = 1; //Set this before the challenge to one or two (ramp challenge or dropping challenge)
float duration, distance;
Servo dropper;
float currentForwardTarget;
float currentReverseTarget;
float currentTarget;
float distanceToleranceMeters = 0.2;
const float KP = 400, KI = 40, KD = 30;
const float trialOneForwardTargetMeters = 0;
const float trialOneReverseTargetMeters = 2.5;

const float trialTwoForwardTargetMeters = 2;
const float trialTwoSlowZoneMeters = 1.5;
const float trialTwoReverseTargetMeters = 0;

//Minimum PWM (speed) required for the drivetrain to stil move
const int minPWM = 50; // This value needs to be experimentally determined still

//Initalize pins for input and output
void setup() {
  switch (trial) {
  case 1:
    currentForwardTarget = trialOneForwardTargetMeters;
    currentReverseTarget = trialOneReverseTargetMeters;
    break;

  case 2:
    currentForwardTarget = trialTwoForwardTargetMeters;
    currentReverseTarget = trialTwoReverseTargetMeters;
    break;
  }

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dropper.attach(servoPin); 
  //Start the serial monitor during debugging
  if (debug == 1){
  Serial.begin(9600);
  }
}

void loop() {
  
  sense_distance();
  //motor_speed(pwm int);
  delay(25);

}
