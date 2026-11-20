//The control scheme here works for both the L298N and L6205 controllers

void motor_forward(){
  digitalWrite(motorFWD, HIGH);
  digitalWrite(motorREV, LOW);
  analogWrite(light1, 0);
  analogWrite(light2, 0);
  if (debug == 1){Serial.println("# Motor turning forward");}
}
void motor_reverse(){
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, HIGH);
  analogWrite(light1, 125);
  analogWrite(light2, 125);
  if (debug == 1){Serial.println("# Motor reversing");}

  
}
void motor_brake(){
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, LOW);
  digitalWrite(motorSpeed, HIGH);
  analogWrite(light1, 125);
  analogWrite(light2, 125);
  if (debug == 1){Serial.println("# Motor Braking");}
}

void motor_speed(int speedPWM){
  analogWrite(motorSpeed, speedPWM);
}
