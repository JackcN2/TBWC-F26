//Rotates the dropper 180 degrees and waits the set delay time
void drop_payload() {
  //Pwm output to set the servo to 180 degrees (maximum rotation)
  dropper.write(180);
  delay(DROP_WAIT);
  //Print to confirm dropping if in debug mode
  if (debug == 1) { Serial.println("Dropped!"); }
}

//Resets the dropper to 0 degrees
void reset_dropper() {
//Pwm output to set the servo to 0 degrees
  dropper.write(0);
  //Print to confirm dropper reset if in debug mode
  if (debug == 1) { Serial.println("Dropper reset"); }
}
