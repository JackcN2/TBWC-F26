//Library used to control the servo position for the dropper arm
#include <Servo.h>

//Pin defitions for input/output devices

//Transmit and detect pin for the ultrasonic sensor
const int trigPin = 11;
const int echoPin = 10;

//Data pin for the servo PWM siginal
const int servoPin = 12;

//Forward and reverse pins for the motor controller
const int motorFWD = 4;
const int motorREV = 6;

//PWM pin to set the motor speed
const int motorSpeed = 3;

//Pin to turn on and off the light used to indicate which state the TBWC is currently in
const int light1 = 5;


//Flags to set before running

// Debug mode 0 = no serial print output, mode 1 = serial print output for testing
const bool debug = 0;  

//Choose if the TBWC is running trial 1 or 2 (1 is speed run 2 is recyclable delivery)
const int trial = 2;  

//Distances to follow during the trials
//Given in meters
//Offset is the rough distance between the sensor and the front of the TBWC
//Tolerance can be used to adjust sensor inacuracy but was not needed in the end
const float trialOneForwardTargetMeters = 0.60;
const float trialTwoForwardTargetMeters = 1.5;  
const float targetInSlowZone = 0.28;            
const float distanceToleranceMeters = 0;
const float distanceOffsetMeters = 0.1;        
    
 

//amount of distance readings to take from the ultrasonic sensor before treating them as valid input
const int CONFIRM_COUNT = 2;

//Timing constants during the trials
const unsigned long START_DELAY_MS = 1000;
const unsigned long RAMP_DELAY_MS = 4500;
const unsigned long DROP_WAIT = 2000;
const unsigned long SETTLE_MS = 500;           
const unsigned long SENSOR_WAIT_MS = 1500;     
const unsigned long RETURN_TIMEOUT_MS = 8000; 

//Set up for variables modified at runtime
float distance = 0;           
bool newDistanceSample = false; 
bool distanceReady = false;   
int belowCount = 0;

//Initialize the servo object for the dropper
Servo dropper;

//State enum, DRIVING and DONE are used in both trials, SLOW_ZONE is used in trial 2 only
enum State { DRIVING, SLOW_ZONE, DONE };
//Default state is DRIVING
State state = DRIVING;

//Helper function to check and confirm that the TBWC is below the current distance threshold and should move to the next state
bool belowThreshold(float limit) {
  if (!distanceReady) return false; // Ignore if no valid reading is given to prevent incorrect stops from faulty data and to save processing power
  if (newDistanceSample) {
    newDistanceSample = false;
    //Wait until the required amount of distance samples is met to confirm 
    belowCount = (distance < limit) ? belowCount + 1 : 0;
  }
  return belowCount >= CONFIRM_COUNT;
}

void setup() {
  //Set the appropriate mode for each I/O device
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorFWD, OUTPUT);
  pinMode(motorREV, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(light1, OUTPUT);
//Initialize the servo on the correct pin and start with it reset to 0 degrees
  dropper.attach(servoPin);
  reset_dropper();

//Start the serial monitor for text output in debug mode
  if (debug == 1) {
    Serial.begin(115200);
  }
  //Wait the start delay to allow the person setting up the TBWC to clear the area
  delay(START_DELAY_MS);

 //Wait until a valid distance reading is taken to start the program
 //Start after the timeout even if no reading is taken to prevent the program from stalling 
  unsigned long t0 = millis();
  while (!distanceReady && millis() - t0 < SENSOR_WAIT_MS) {
    sense_distance();
  }
//Initialize sensor variables
  newDistanceSample = false;
  belowCount = 0;

//Run the starting sequence for the speed run trial
//The LED (light1) is on to indicate timed driving mode and turns off to indicate distance based driving mode (in both trials)

  if (trial == 1) {
    analogWrite(light1, 190);
    motor_forward(); // Drive forward for the amount of time needed to cross the ramp in the forward direction (needed as the sensor cannot take valid readings during this phase)
    motor_speed(255);
    delay(RAMP_DELAY_MS);
    analogWrite(light1, 0);
  }
  //Trial 2 starting sequence. Same as trial 1 but for a shorter time just to get the TBWC close to the bay before it starts taking distance measurements
  if (trial == 2) {
    analogWrite(light1, 190);
    motor_forward();
    motor_speed(255);
    delay(4000);
    analogWrite(light1, 0);
  }
}

//Main code loop, runs untill TBWC is turned off
void loop() {
  sense_distance(); // Continually poll for the distance reading. Described in more detail in sensor.ino

  if (state == DONE) return; //TBWC has finished the set trial. Do nothing 
  //Check the state machine for each trial and do the corisponding action
  if (trial == 1) run_trial_one();
  if (trial == 2) run_trial_two();
}

//Trial 1 state machine
void run_trial_one() {
  //If within the stopping distance after the ramp: reverse the motor direction, enter timed driving mode to to return to the starting line, then brake and enter the DONE state
  if (belowThreshold(trialOneForwardTargetMeters + distanceToleranceMeters + distanceOffsetMeters)) { 
    analogWrite(light1, 190);
    motor_reverse();
    motor_speed(255);
    delay(7000);//Longer than the forward timed mode to account for switching acceleration directions and inefficiency introduced by crossing the ramp with the back axle first
    analogWrite(light1, 0);


    motor_brake();
    state = DONE;
  }
}

//Trial 2 state machine
void run_trial_two() {
  if (state == DRIVING) {
    //Drive at a slower speed once close to the recycling bay to prevent hitting the walls or an early payload release
    if (belowThreshold(trialTwoForwardTargetMeters + distanceToleranceMeters + distanceOffsetMeters)) {
      motor_speed(200);
      state = SLOW_ZONE;
      if (debug == 1) { Serial.println("In Slowzone");}
      belowCount = 0; 
    }
  } else if (state == SLOW_ZONE) {
    //Once in SLOW_ZONE state if the TBWC has arrived at the recycling bin then brake, drop the payload, and enter the DONE state
    if (belowThreshold(targetInSlowZone + distanceToleranceMeters + distanceOffsetMeters)) {
      motor_brake();
      delay(SETTLE_MS);
      drop_payload();
      motor_brake();
      analogWrite(light1, 0);
      state = DONE;
    }
  }
}

