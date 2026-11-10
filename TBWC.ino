//Included libraries
#include <Servo.h>

//Pin definitions 
const int trigPin = 9;
const int echoPin = 10;
const int servoPin = 12;

//Global variables
const bool debug = 1; //Set to 0 when running in the real demonstration to save processing power (disables serial prints)
float duration, distance;
Servo dropper;

//Initalize pins for input and output
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  dropper.attach(servoPin); 
  //Start the serial monitor during debugging
  if (debug == 1){
  Serial.begin(9600);
  }
}

void loop() {
  
  sense_distance();
  delay(50);

}
