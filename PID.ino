//Set the PID controllers distance target based on direction (1 is fwd 2 is rev)
void set_target(int direction){
  switch(direction){
    case 1:
    currentTarget = currentForwardTarget; 
    break;

  case 2:
    currentTarget = currentReverseTarget;
    break;
  }
}

//Ignore this for now it will not do anything untill the motion profile is created.
void distance_error(){
  float distanceToTargetMeters = distance - currentTarget;
  float err = distanceToTargetMeters;
}