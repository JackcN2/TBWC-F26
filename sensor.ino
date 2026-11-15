unsigned long previousMillis = 0;
const long interval = 50;
const unsigned long echoTimeoutUs = 30000; //5 meters, Timeout stops pulsein() from blocking for a full second if it fails

void sense_distance() {
  //Microsecond delays are insignificant
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, echoTimeoutUs);
 
  if (duration > 0) {
    //Read in cm then converted to meters
    distance = ((duration*.0343)/2)/100;
    sampleTimeMs = currentMillis;
    newDistanceSample = true;
  }

  }
 

}
