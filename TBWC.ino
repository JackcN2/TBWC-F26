//Pin definitions and global variables
const int trigPin = 9;
const int echoPin = 10;
const bool debug = 1; //Set to 0 when running in the real demonstration to save processing power (disables serial prints)
float duration, distance;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  if (debug == 1){
  Serial.begin(9600);
  }
}

void loop() {
  
  sense_distance();
  delay(100);

}
