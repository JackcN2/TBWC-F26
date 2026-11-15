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
const bool debug = 1; //Set to 0 when running in the real demonstration to save processing power 
const int trial = 1; //Set this before the challenge to 1 or 2 (ramp challenge or dropping challenge)
float duration, distance;
Servo dropper;
float currentForwardTarget;
float currentReverseTarget;
float currentTarget;
float distanceToleranceMeters = 0.1;

const float trialOneForwardTargetMeters = 0;
const float trialOneReverseTargetMeters = 2.5;

const float trialTwoForwardTargetMeters = 2;
const float trialTwoSlowZoneMeters = 1.5;
const float trialTwoReverseTargetMeters = 0;


bool newDistanceSample = false; 
unsigned long sampleTimeMs = 0;


//All of these need real tuning still
const int minPWM = 50;
const float VEL_FULL_PWM = 1.0;      
const float VEL_CRUISE = 0.7;         //VMAX that we request, VEL_FULL_PWM is the real VMAX
const float VEL_SLOW_ZONE = 0.3;     
const float VEL_CREEP = 0.15;      
const float ACCEL_MAX = 0.6;          
const float STOP_OFFSET_METERS = 0.0; //Only use if the cart rolls after braking


const float KP = 120, KI = 40, KD = 0; 
const float I_LIMIT = 80;             
const float VEL_FILTER_ALPHA = 0.4;    
const float BRAKE_OVERSPEED = 0.2;  

const unsigned long START_DELAY_MS = 1000;    //1 second
const unsigned long SETTLE_MS = 500;        
const unsigned long SETTLE_TIMEOUT_MS = 2000;
const float VEL_STOPPED = 0.05;             
const unsigned long SENSOR_LOSS_MS = 400;     
const bool dropEnabled = (trial == 2);     

//init pins for input and output
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
  pinMode(motorFWD, OUTPUT);
  pinMode(motorREV, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  dropper.attach(servoPin); 
  //Start the serial monitor in debug mode 
  if (debug == 1){
  Serial.begin(115200); 
  Serial.println("ms,state,dist_m,vel_mps,vref_mps,pwm");
  }

  control_init();
}

void loop() {
  
  sense_distance();   
  control_update();  

}
