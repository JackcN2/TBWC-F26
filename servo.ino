//Rotates the dropper 180 degrees and waits the set delay time
void drop_payload() {
  dropper.write(180);
  delay(DROP_WAIT);
  if (debug == 1) { Serial.println("Dropped!"); }
}

//Resets the dropper to 0 degrees
void reset_dropper() {
  dropper.write(0);
  if (debug == 1) { Serial.println("Dropper reset"); }
}
