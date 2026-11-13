//The control scheme here works for both the L298N and L6205 controllers

float speedDebug;
void motor_forward(){
  digitalWrite(motorFWD, HIGH);
  digitalWrite(motorREV, LOW);
  if (debug == 1){Serial.println("Motor turning forward");}
}
void motor_reverse(){
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, HIGH);
  if (debug == 1){Serial.println("Motor reversing");}
  
}
void motor_brake(){
  digitalWrite(motorFWD, LOW);
  digitalWrite(motorREV, LOW);
  digitalWrite(motorSpeed, HIGH);  
  if (debug == 1){Serial.println("Motor Braking");}
}

void motor_speed(int speedPWM){
  analogWrite(motorSpeed, speedPWM);
  if (debug == 1){speedDebug = ((speedPWM/255)*100);Serial.print("Motor Speed set to");Serial.print(speedDebug);}
}