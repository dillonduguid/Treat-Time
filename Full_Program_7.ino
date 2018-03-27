//requires IR Remote Library
#include <IRremote.h> //import IRremote.h file from IR library

int RECV_PIN = 11; //set IR reciever pin
IRrecv irrecv(RECV_PIN); //IR reciever object instantiated with pin parameters
decode_results results; //results variable

//initialising screen segment variables
int power = 2;
int topMiddle = 3;
int power2 = 4;
int topRight = 5;
int bottomRight = 6;
int bottomMiddle = 7;
int bottomLeft = 8;
int topLeft = 9;
int middleMiddle = 10;

//servo pins initialised with corersponding pins
int servoPins[4] = {15, 16, 17, 18};

//initialise some variables used throughout program
int counter = 1; //this variable stores how many times the system will turn, increasing or decreasing how many treats are dispensed
int count = 0;
int delaytime = 8;
long previousTime = 0;
bool timer = true;

int threshold = 50;
int sensor = 0;

//initialise led constant pins
const int redLed = 12;
const int greenLed = 14;


//setup runs once, sets output pin mode for each pin used and sets default value on 7 segment screen
void setup() {
  screenDecision(counter); //call screen changing function with default counter value (set to 1)
  
  irrecv.enableIRIn(); // Start the receiver

  //Setting screen segment pins to output
  pinMode(power, OUTPUT);
  pinMode(power2, OUTPUT);
  pinMode(topMiddle, OUTPUT);
  pinMode(topRight, OUTPUT);
  pinMode(bottomRight, OUTPUT);
  pinMode(bottomMiddle, OUTPUT);
  pinMode(bottomLeft, OUTPUT);
  pinMode(topLeft, OUTPUT);
  pinMode(middleMiddle, OUTPUT);

  //Set servo pins to output
  for(int i = 0; i < 4; i++){
    pinMode(servoPins[i], OUTPUT);
  }

  //Set led pins to output
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);

  digitalWrite(greenLed, HIGH);
}

//Loop runs forever until program stops
void loop() {

  sensor = 0;
  long currentTime = millis(); //get current time in milli seconds

  //this section keeps looking for IR signals, then sends the value to the decide function
  if (irrecv.decode(&results)) { 

    irrecv.resume(); // Receive the next value

    //decide function called with results.value as parameter
    decide(results.value);
  }
  
  //if the timer is on
  Serial.println(currentTime - previousTime);
  if (timer == true) {
    if ((currentTime - previousTime) > 10000 ) { //if the current system time (taken away from the previous time the timer was set) is greater than the predefined time value (10 seconds - can be changed later based on user preference)
      digitalWrite(redLed, HIGH); //turn on red LED
      sensor = analogRead(A5); //get sensor input if there is any
      Serial.println(sensor);
      if (sensor > threshold) { //check if the sensor input is greater than the force threshold
        turnDecision(sensor); //call turnDecision function with sensor parameter
        previousTime = currentTime; //assign previous time to the current program time 
      }
    }
    //otherwise, turn of red LED
    else {
      digitalWrite(redLed, LOW);
    }
  }
  //otherwise
  else {
    sensor = analogRead(A5); //receieve sensor input 
    if (!(sensor > threshold)) { //if the sensor value doesn't break the threshold
      digitalWrite(redLed, HIGH); //light up red LED for 500 milliseconds
      delay(500);
      digitalWrite(redLed, LOW);
    }
    turnDecision(sensor); //call turnDecision function with sensor variable as parameter
  }

  //if timer is on, turn on green LED
}

//function 'turnDecision' returns no value, receives an integer 'sensor' as parameter
void turnDecision(int sensor) {

  //only let the motor turn if the number of times it should turn is greater than 0
  if (counter > 0) {
    for (int i = 0; i < counter; i++) {
      turn(sensor); //call turn function with sensor parameter
    }
  }
  
}

//function 'turn' returns no value, receives an integer 'sensor' as the parameter
void turn(int sensor) {

  //if the sensor breaks the threshold
  if (sensor > threshold) {

    //set angle to 130 degrees
    int angle = 130;

    turnOff(); //stop turning
    while (angle > 0) { //loop through each degree
      forward(); //turn the motor
      angle--; //decrement the angle
    }
    
  }
}

/*Function 'decide' returns no value, receives an integer 'value' as parameter
  This method will take in a IR signal and decode it by checking if any of the pre-defined button values match the input*/
void decide(int value) {

  //recieve signal, check what button it is, then change counter based on the corresponding input.
  
  //if power button is clicked
  if (value == -23971 || value == 9755 || value == -5033 || value == 9755 || value == -32741 || value == -31078 ||
      value == -2423 || value == -10756 || value == 18107) {
    counter = 0;
  }
  //if '1' button is clicked
  else if (value == 12495 || value == -16833 || value == -5289 || value == -30310 || value == 4382
           || value == -28348 || value == -3698) {
    counter = 1;
  }
  //if '2' button is clicked
  else if (value == -7177 || value == 6375 || value == -14049 || value == -8884 || value == 12986 || value == 1640
           || value == -4915 || value == -2136 || value == 10643 || value == 2181) {
    counter = 2;
  }
  //if '3' button is clicked
  else if (value == 539 || value == 31365 || value == 18163 || value == 31365) {
    counter = 3;
  }

  //increment counter value
  if (value == -28561 || value == -15918 || value == -24926 || value == 17310 || value == -30813 || value == -17025
      || value == -14130 || value == -15260 || value == -5799 || value == -28561 || value == 16197) {
    counter ++;
    if (counter > 3) { //make sure counter isn't larger than the maximum
      counter = 3;
    }
  }
  //decrement counter value
  else if (value == -65 || value == 6733 || value == -8161 || value == -16069 || value == 23205 || value == 25628
           || value == -15411) {
    counter--;
    if (counter < 0) { //make sure counter isn't lower than the minimum
      counter = 0;
    }
  }

  //turn timer on or off
  if (value == -7651 || value == 28031 || value == 26497 || value == -1) {
    if (timer == true) { //if timer is currently on
      timer = false; //turn timer off
      digitalWrite(greenLed, LOW);

    }
    else { //otherwise, turn timer on
      timer = true;
      digitalWrite(greenLed, HIGH);
    }
    
    
  }

  screenDecision(counter); //call screenDecision function with counter parameter

  //if 'force motor turn' button is clicked
  if (counter > 0) {
    if (value == 765 || value == 19224 || value == 19227 || value == 11813 || value == 30291 || value == 11127 || value == -15682
        || value == 1869 || value == -18544 || value == -29253) {
      turnDecision(threshold + 1); //call turnDecision fucntion to turn the motor.
    }
  }

}

/*Function 'screenDecision' returns nothing when called, receives integer 'number' as parameter
  Method will decide what value is to be displayed on the screen, based on what the counter value is*/
void screenDecision(int number) {
  
  screenOff(); //turn off the seven-segment display

  //Specific segments are lit up in order to display viable information (counter value)
  
  //if the number to be displayed is 0, light up neccesary segments on the screen
  if (number == 0) {
    digitalWrite(topMiddle, HIGH);
    digitalWrite(topLeft, HIGH);
    digitalWrite(topRight, HIGH);
    digitalWrite(bottomLeft, HIGH);
    digitalWrite(bottomRight, HIGH);
    digitalWrite(bottomMiddle, HIGH);
  }
  else if (number == 1) {
    digitalWrite(bottomRight, HIGH);
    digitalWrite(topRight, HIGH);
  }
  else if (number == 2) {
    digitalWrite(topMiddle, HIGH);
    digitalWrite(topRight, HIGH);
    digitalWrite(middleMiddle, HIGH);
    digitalWrite(bottomLeft, HIGH);
    digitalWrite(bottomMiddle, HIGH);
  }
  else if (number == 3) {
    digitalWrite(topMiddle, HIGH);
    digitalWrite(topRight, HIGH);
    digitalWrite(middleMiddle, HIGH);
    digitalWrite(bottomMiddle, HIGH);
    digitalWrite(bottomRight, HIGH);
  }
}

/*Function 'screenOff' returns no value, receives no parameters
  Method will turn off all segments on the seven-segment display*/
void screenOff() {
  digitalWrite(bottomRight, LOW);
  digitalWrite(bottomLeft, LOW);
  digitalWrite(middleMiddle, LOW);
  digitalWrite(topMiddle, LOW);
  digitalWrite(topLeft, LOW);
  digitalWrite(topRight, LOW);
  digitalWrite(bottomMiddle, LOW);
  digitalWrite(topMiddle, LOW);
}

/*Functon 'turnOff' returns no value, receives no parameters
  Method stops motor from turning*/
void turnOff() {
  digitalWrite(servoPins[0], LOW);
  digitalWrite(servoPins[1], LOW);
  digitalWrite(servoPins[2], LOW);
  digitalWrite(servoPins[3], LOW);
}

/*Function 'forward' returns no value, receives no paramters
  Method moves motor, spaced out by a delay for each turn*/
void forward() {
  digitalWrite(servoPins[0], HIGH);
  digitalWrite(servoPins[1], LOW);
  digitalWrite(servoPins[2], LOW);
  digitalWrite(servoPins[3], LOW);
  delay(delaytime); //delay motor
  
  digitalWrite(servoPins[0], LOW);
  digitalWrite(servoPins[1], HIGH);
  digitalWrite(servoPins[2], LOW);
  digitalWrite(servoPins[3], LOW);
  delay(delaytime); //delay motor
  
  digitalWrite(servoPins[0], LOW);
  digitalWrite(servoPins[1], LOW);
  digitalWrite(servoPins[2], HIGH);
  digitalWrite(servoPins[3], LOW);
  delay(delaytime); //delay motor
  
  digitalWrite(servoPins[0], LOW);
  digitalWrite(servoPins[1], LOW);
  digitalWrite(servoPins[2], LOW);
  digitalWrite(servoPins[3], HIGH);
  delay(delaytime); //delay motor

  //stop motor from turning
  turnOff();
}
