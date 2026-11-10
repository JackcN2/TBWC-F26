unsigned long previousMillis = 0;
const long interval = 50;

void sense_distance() {
  //Microsecond delays are insignificant to the program and as such blocking delay calls are used for simplicity
  unsigned long currentMillis = millis();
//Non blocking delay by a set interval (currently 50ms, may be adjusted) to avoid wasting processing power on distance scanning
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  //Distance is read in cm and converted to meters for consistency
  distance = ((duration*.0343)/2)/100;
  if (debug == 1){
  Serial.print("Distance (meters): ");
  Serial.print(distance);
  }
  }
 

}