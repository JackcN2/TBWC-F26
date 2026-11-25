//Helper functions for commanding the motor controller
//If in debug mode print what action was taken when each function is called
//The control scheme here works for both the L298N and L6205 controllers

//Set the forward pin to HIGH and the reverse pin to LOW to make the motor turn forward
void motor_forward() {
  digitalWrite(motorFWD, HIGH);
  digitalWrite(motorREV, LOW);


  if (debug == 1) { Serial.println("# Motor turning forward"); }
}

//Set the forward pin to LOW and the reverse pin to HIGH to make the motor turn in reverse
void motor_reverse() {
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, HIGH);


  if (debug == 1) { Serial.println("# Motor reversing"); }
}

//Set both directions to LOW and speed to maximum to make the motor brake
//Max speed is needed for the motor to fight changes in velocity instead of free spinning
void motor_brake() {
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, LOW);
  digitalWrite(motorSpeed, HIGH);

  if (debug == 1) { Serial.println("# Motor Braking"); }
}

//Set the motor speed
//Speed is a PWM siginal where 0 represents minimum and 255 represents maximum
void motor_speed(int speedPWM) {
  analogWrite(motorSpeed, speedPWM);
}
