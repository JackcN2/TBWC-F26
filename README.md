The code is organized into 3 sections: 

Input/Output 

PID controller/state machine 

Main

Each device connected to the Arduino (The motor controller, the servo, the ultrasonic sensor) has an I/O file which provides functions for the rest of the code to interface with it. This allows the main code to be hardware agnostic, and means less code needs to be changed if a device has to be swapped out. 

The PID controller section is still under development. When complete it will provide closed loop control to the motor based on the carts current speed (calculated from its distance delta over a time period) and the remaining distance it must travel. The state machine controls what action the cart is currently targeting (e.g. driving towards the finish line, reversing, waiting while dropping the payload). The PID loop also uses a motion profile we will tune to establish certain physical values like maximum cart velocity, maximum cart acceleration, minimum PWM signal sent to the motor for the cart to still move, distance offsets, etc.  

The main file (TBWC.ino) provides global variables to let the PID loop and I/O functions communicate, and controls the distance polling speed as well as the starting values for the state machines. Due to the way Arduino code works it also initializes all I/O pins on startup, and runs the main infinite loop that allows the other sections to function when needed. Optionally, debug data and statements can be sent to the serial monitor from main. These will be turned off during the actual trials to conserve processing power.