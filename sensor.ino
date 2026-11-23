

unsigned long previousMillis = 0;
const long interval = 50;
const unsigned long echoTimeoutUs = 30000;  
const float minValidMeters = 0.02;          

const int N = 5;
float buf[N];
int bufCount = 0;
int bufIdx = 0;

float medianOfBuffer() {
  float tmp[N];
  for (int i = 0; i < N; i++) tmp[i] = buf[i];
  for (int i = 1; i < N; i++) {
    float key = tmp[i];
    int j = i - 1;
    while (j >= 0 && tmp[j] > key) {
      tmp[j + 1] = tmp[j];
      j--;
    }
    tmp[j + 1] = key;
  }
  return tmp[N / 2];
}

void sense_distance() {
  unsigned long now = millis();
  if (now - previousMillis < interval) return;
  previousMillis = now;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long dur = pulseIn(echoPin, HIGH, echoTimeoutUs);
  if (dur == 0) return; 


  float m = ((dur * 0.0343) / 2) / 100.0;
  if (m < minValidMeters) return;

  buf[bufIdx] = m;
  bufIdx = (bufIdx + 1) % N;
  if (bufCount < N) bufCount++;

  if (bufCount == N) {
    distance = medianOfBuffer();
    distanceReady = true;
    newDistanceSample = true;
    if (debug == 1) {
      Serial.println(distance);
    }
  }
}
