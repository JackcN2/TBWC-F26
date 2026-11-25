
//Timing variable used for non blocking delays
unsigned long previousMillis = 0;

//Polling interval (in miliseconds)
const long interval = 50;

//Max timeout to wait before getting a distance echo back
//Prevents the program from stalling
const unsigned long echoTimeoutUs = 30000;  

//Minimum distance that can be read by the HC-SR04 distance sensor
const float minValidMeters = 0.02;          

//Initialize variables for the distance buffer
//Takes 5 samples before takingg the median
const int N = 5;
float buf[N];
int bufCount = 0;
int bufIdx = 0;

//Read the distance into a temporary array to preserve the original order of readings
float medianOfBuffer() {
  float tmp[N];
  for (int i = 0; i < N; i++) tmp[i] = buf[i];
  //Basic insertion sort for speed
  //Sorting the values is needed so a median value can be taken
  for (int i = 1; i < N; i++) {
    float key = tmp[i];
    int j = i - 1;
    while (j >= 0 && tmp[j] > key) {
      tmp[j + 1] = tmp[j];
      j--;
    }
    tmp[j + 1] = key;
  }
  //Return the median
  return tmp[N / 2];
}

//Entry point from the main file
void sense_distance() {
  //Wait until the timing interval has passed to take a new reading to prevent polling too fast and crashing or stalling the program
  unsigned long now = millis();
  if (now - previousMillis < interval) return;
  previousMillis = now;

//Send pulse to get distance feedback
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); //These calls are blocking but microseconds are insignificant to overall program timing 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
//Ignore invalid readings
  unsigned long dur = pulseIn(echoPin, HIGH, echoTimeoutUs);
  if (dur == 0) return; 

//Convert the raw pulse input to a value in meters for consistency
  float m = ((dur * 0.0343) / 2) / 100.0;
  //Ignore values that are too small to be valid
  if (m < minValidMeters) return;
//Collect distance samples into a buffer until enough are taken to be valid
  buf[bufIdx] = m;
  bufIdx = (bufIdx + 1) % N;
  if (bufCount < N) bufCount++;

//Once the buffer is full take the median reading from the buffer to eliminate noise and momentarily incorect readings
  if (bufCount == N) {
    distance = medianOfBuffer();
    //Update flags to siginal to the main program loop that there is a valid distance reading to be used
    distanceReady = true;
    newDistanceSample = true;
    //Output the distance read in meters if in debug mode
    if (debug == 1) {
      Serial.println(distance);
    }
  }
}
