const int ST_WAIT      = 0; 
const int ST_DRIVE_FWD = 1;  //fwd target
const int ST_DROP      = 2;  //drop paylod
const int ST_DRIVE_REV = 3;  //rev target
const int ST_DONE      = 4;  //finished state, brake and wait for power down

int state = ST_WAIT;
unsigned long stateStartMs = 0;

int fwdSign = 1;   
int travelDir = 1;   


float distBuf[3];                  //last three raw readings 
int sampleCount = 0;
float distFiltered = 0;            //median distance
float prevDistFiltered = 0;
unsigned long prevSampleMs = 0;
float velMeasured = 0;           
float loopDt = 0.05;             


float velProfile = 0;            
float iTerm = 0;                   //integral term, in PWM counts
float prevVelAlong = 0;
float vRefDebug = 0;
int pwmDebug = 0;

//
int motorMode = -1;             
int motorPWM = -1;


//Set the PID controllers distance target based on direction (1 is fwd 2 is rev)
void set_target(int direction){
  switch(direction){
    case 1:
    currentTarget = currentForwardTarget; 
    break;

  case 2:
    currentTarget = currentReverseTarget;
    break;
  }
}

float distance_remaining(){
  return fwdSign * travelDir * (currentTarget - distFiltered);
}

float median3(float a, float b, float c){
  float t;
  if (a > b) { t = a; a = b; b = t; }
  if (b > c) { t = b; b = c; c = t; }
  if (a > b) { t = a; a = b; b = t; }
  return b;
}

//mode: 0 = brake, 1 = forward, 2 = reverse. pwm: 0-255 
void drive_motor(int mode, int pwm){
  if (mode != motorMode){
    if (mode == 1) motor_forward();
    else if (mode == 2) motor_reverse();
    else motor_brake();
    motorMode = mode;
    motorPWM = -1;               //braking leaves the speed pin at high, so force a fresh analogWrite to prevent a stale speed
  }
  if (mode != 0 && pwm != motorPWM){
    motor_speed(pwm);
    motorPWM = pwm;
  }
}


//return true when a new filtered sample is there
bool update_measurements(){
  if (!newDistanceSample) return false;
  newDistanceSample = false;

  distBuf[2] = distBuf[1];
  distBuf[1] = distBuf[0];
  distBuf[0] = distance;
  if (sampleCount < 3) sampleCount++;

  if (sampleCount < 3){            //not enough history for the median yet
    distFiltered = distance;
    prevDistFiltered = distance;
    prevSampleMs = sampleTimeMs;
    return false;
  }

  distFiltered = median3(distBuf[0], distBuf[1], distBuf[2]);

  float dt = (sampleTimeMs - prevSampleMs) / 1000.0;
  if (dt <= 0) return false;

  //Speed from the distance delt
  float vRaw = fwdSign * (distFiltered - prevDistFiltered) / dt;
  velMeasured += VEL_FILTER_ALPHA * (vRaw - velMeasured);   //Filter the distance delta to reduce noise
  prevDistFiltered = distFiltered;
  prevSampleMs = sampleTimeMs;
  loopDt = dt;
  return true;
}

//Returns the speed the cart should have right now.
//accelerates at ACCEL_MAX up to vCap
float motion_profile(float remaining, float vCap, float dt){
  //Stop at the edge of the tolerance band (plus any experimentally found offset), not at the target itself
  float stopDist = remaining - distanceToleranceMeters - STOP_OFFSET_METERS;
  if (stopDist < 0) stopDist = 0;
  float vDecel = sqrt(2.0 * ACCEL_MAX * stopDist);

  float vGoal = min(vCap, vDecel);
  if (vGoal < VEL_CREEP) vGoal = VEL_CREEP;

  if (vGoal > velProfile){         
    velProfile += ACCEL_MAX * dt;
    if (velProfile > vGoal) velProfile = vGoal;
  } else {                        
    velProfile = vGoal;
  }
  return velProfile;
}


// Returns PWM 0-255.
int velocity_pid(float vRef, float vAlong, float dt){
  float err = vRef - vAlong;


  float ff = minPWM + (255.0 - minPWM) * vRef / VEL_FULL_PWM;

  float pTerm = KP * err;
  float dTerm = -KD * (vAlong - prevVelAlong) / dt;  
  prevVelAlong = vAlong;

  float out = ff + pTerm + iTerm + dTerm;

  //Stops the I term from creating a negative feedback loop
  bool pushingHigh = (out >= 255) && (err > 0);
  bool pushingLow  = (out <= 0) && (err < 0);
  if (!pushingHigh && !pushingLow) iTerm += KI * err * dt;
  iTerm = constrain(iTerm, -I_LIMIT, I_LIMIT);

  out = ff + pTerm + iTerm + dTerm;

  //Avoid dropping below the minium pwm required to move
  if (out < minPWM && vAlong <= vRef) out = minPWM;

  return (int)constrain(out, 0, 255);
}

//State machine
void enter_state(int newState){
  state = newState;
  stateStartMs = millis();
  iTerm = 0;
  velProfile = VEL_CREEP;

  switch (newState){
    case ST_DRIVE_FWD:
      travelDir = 1;
      set_target(1);
      velMeasured = 0;             //States must always start stationary
      prevVelAlong = 0;
      break;

    case ST_DRIVE_REV:
      travelDir = -1;
      set_target(2);
      velMeasured = 0;
      prevVelAlong = 0;
      break;

    default:                       //Default state is to brake
      drive_motor(0, 0);
      break;
  }

  if (debug == 1){
    Serial.print("# state -> ");
    Serial.println(newState);
  }
}


void run_drive(float dt){
  float remaining = distance_remaining();

  
  if (remaining <= distanceToleranceMeters){
    drive_motor(0, 0);
    if (state == ST_DRIVE_FWD) enter_state(ST_DROP);
    else enter_state(ST_DONE);
    return;
  }

  //Taper the cart speed to match VMAX, in trial 2 cap speed at the slowzone speed
  float vCap = VEL_CRUISE;
  if (trial == 2 && state == ST_DRIVE_FWD){
    float toZone = fwdSign * (trialTwoSlowZoneMeters - distFiltered);   //positive until zone entry
    if (toZone < 0) toZone = 0;
    float vApproach = sqrt(VEL_SLOW_ZONE * VEL_SLOW_ZONE + 2.0 * ACCEL_MAX * toZone);
    vCap = min(vCap, vApproach);
  }

  float vRef = motion_profile(remaining, vCap, dt);
  float vAlong = travelDir * velMeasured;          //speed in the direction we are trying to go
  int pwm = velocity_pid(vRef, vAlong, dt);

  vRefDebug = vRef;
  pwmDebug = pwm;

  //Saftey condition to cause braking if the cart is overspeed
  if (vAlong > vRef + BRAKE_OVERSPEED){
    pwmDebug = 0;
    drive_motor(0, 0);
  } else {
    drive_motor(travelDir > 0 ? 1 : 2, pwm);
  }
}

//Reset to default states when the cart starts.
void control_init(){
  fwdSign = (currentForwardTarget > currentReverseTarget) ? 1 : -1;
  reset_dropper();
  drive_motor(0, 0);
  enter_state(ST_WAIT);
}


void control_update(){
  if (!update_measurements()){
    //Saftey function to stop if the ultrasonic sensor fails or disconects.
    if ((state == ST_DRIVE_FWD || state == ST_DRIVE_REV) && (millis() - sampleTimeMs > SENSOR_LOSS_MS)){
      drive_motor(0, 0);
    }
    return;
  }

  vRefDebug = 0;
  pwmDebug = 0;

  switch (state){
    case ST_WAIT:
      if (sampleCount >= 3 && (millis() - stateStartMs >= START_DELAY_MS)) enter_state(ST_DRIVE_FWD);
      break;

    case ST_DRIVE_FWD:
    case ST_DRIVE_REV:
      run_drive(loopDt);
      break;

    case ST_DROP:
      drive_motor(0, 0);
      //Wait untill the cart has stopped before dropping
      if ((millis() - stateStartMs >= SETTLE_MS) &&
          (fabs(velMeasured) < VEL_STOPPED || millis() - stateStartMs >= SETTLE_TIMEOUT_MS)){
        if (dropEnabled){
          drop_payload();         //Intentionally blocking function as no other actions must be taken while dropping the payload
          reset_dropper();
        }
        enter_state(ST_DRIVE_REV);
      }
      break;

    case ST_DONE:
      drive_motor(0, 0);
      break;
  }


  if (debug == 1){
    Serial.print(millis());              Serial.print(',');
    Serial.print(state);                 Serial.print(',');
    Serial.print(distFiltered, 3);       Serial.print(',');
    Serial.print(velMeasured, 3);        Serial.print(',');
    Serial.print(travelDir * vRefDebug, 3); Serial.print(',');
    Serial.println(pwmDebug);
  }
}
