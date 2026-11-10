//Rotates the dropper 180 degrees and waits 1.5 seconds for the payload to fall
void drop_payload(){
dropper.write(180);
delay(1500);
if (debug == 1){Serial.println("Dropped!");}
}

//Resets the dropper to 0 degrees
void reset_dropper(){
dropper.write(0);
if (debug == 1){Serial.println("Dropper reset");}
}