#include <EEPROM.h>

//Motor A (Left)
int motorLeftDirection = 12;   //DIR A
int motorLeftSpeed = 6;        //PWM A

//Motor B (Right)
int motorRightDirection = 13;   //DIR B
int motorRightSpeed = 11;       //PWM B

int interruptPin = 2;
int interruptPin2 = 3;
volatile int counter = 0;
volatile int secondCounter = 0;

// Multiplexer
int pinA = 10;
int pinB = 9;
int pinC = 8;

int comOutInPin = A5;

String cmdList = "";

String stateButton = "Pause";

String state = "";


void setup() {
  // multiplexer

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(comOutInPin, INPUT_PULLUP);
  
  //Motor A (Left)
  pinMode(motorLeftDirection, OUTPUT);
  pinMode(motorLeftSpeed, OUTPUT);
  
  //Motor B (Right)
  pinMode(motorRightDirection, OUTPUT);
  pinMode(motorRightSpeed, OUTPUT);


  pinMode(interruptPin, INPUT_PULLUP);
  
  pinMode(interruptPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), count, CHANGE);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), secondCount, CHANGE);
  Serial.begin(9600);
}

void loop() {
  
  // MULTIPLEXER Checks CMD-buttons
  //readStateButtons();
  if(stateButton == "Start"){
    //readCmdButtons();  
    processCmd();
    /*if(state == "forward"){
            drive(HIGH, 100, HIGH, 100, 100);
            delay(2000);
            state = "stop";

    } else if(state == "backward"){
            Serial.print("test1");
            drive(LOW, 100, LOW, 100, 100);
                        delay(2000);

            state = "stop";


    } else if (state == "right"){
            drive(HIGH, 30, LOW, 20, 50);
                        delay(2000);

            state = "stop";



    } else if (state == "left"){
            drive(LOW, 30, HIGH, 20, 50);
            delay(2000);
            state = "stop";

    } else if(state == "stop"){
        delay(100);
        drive(LOW, 0, LOW, 0, 10);

    }
    */
    
    readStateButtons();
    } else if (stateButton == "Pause"){
        Serial.println("Reading input from buttons...");
        
        readCmdButtons();

    }
    
  
  
}

//Drives the robot
void drive(boolean leftDirection, int leftSpeed, boolean rightDirection, int rightSpeed, int distance) {
  //Motor A (Left)
  digitalWrite(motorLeftDirection, leftDirection);
  digitalWrite(motorLeftSpeed, leftSpeed);
  
  //Motor B (Right)
  digitalWrite(motorRightDirection, rightDirection);
  digitalWrite(motorRightSpeed, rightSpeed);

  //Driving distance/time
  delay(distance);

  

}

// void forward(delay){
 // drive(HIGH, 100, HIGH, 100, delay);
// }

void count() {
 counter++;
}

void secondCount(){
  secondCounter++;
}


void resetCounters(){
  counter = 0;
  secondCounter = 0;
}

String changeState(){
  if(stateButton == "Pause"){
    stateButton = "Start";
  } else if (stateButton == "Start") {
    stateButton = "Pause";
    cmdList = "";
  }
  return stateButton;
}



bool readComOutIn(bool C, bool B, bool A) {
  //Defines which channel the comOutIn port is connected to
  digitalWrite(pinA, A);
  digitalWrite(pinB, B);
  digitalWrite(pinC, C);

  //Reads the channel
  boolean channelValue = digitalRead(comOutInPin);
  return channelValue;
}

void readStateButtons(){
  if(readComOutIn(HIGH, LOW,LOW)){
      changeState();
      Serial.println(stateButton);
      delay(500);
      
     while(readComOutIn(HIGH, LOW, LOW));
    }
}

void readCmdButtons() {
  if(readComOutIn(LOW, LOW, LOW)) {
      cmdList += "B";

      delay(100);
      while(readComOutIn(LOW, LOW, LOW));

    }
    
    else if(readComOutIn(LOW, LOW, HIGH)) {
      cmdList += "R";

      delay(100);
      while(readComOutIn(LOW, LOW, HIGH));
    } 
    
    else if(readComOutIn(LOW, HIGH, LOW)) {
      cmdList += "L";
      

      delay(100);
      while(readComOutIn(LOW, HIGH, LOW));
    } 
    
    else if(readComOutIn(LOW, HIGH, HIGH)) {
      cmdList += "F";
      
      delay(100);
      while(readComOutIn(LOW, HIGH, HIGH));
     
    } else if(readComOutIn(HIGH, LOW,LOW)){
      changeState();
      Serial.println(stateButton);
      Serial.println("List : " + cmdList);
      delay(100);
      
     while(readComOutIn(HIGH, LOW, LOW));
    } 
    else if(readComOutIn(HIGH, HIGH ,LOW)){
      Serial.println("STOP");
      delay(100);
     while(readComOutIn(HIGH, HIGH, LOW));
    }
}

void processCmd() {
  for (int i = 0 ; i < cmdList.length() ; i++) {
    char cmd = cmdList.charAt(i);
    Serial.println(cmd);
    if(cmd == 'F') {
      Serial.println("Processing F");
      state = "forward";
      drive(HIGH, 100, HIGH, 100, 100);
      
      delay(50);
      

    } else if (cmd == 'B') {
      Serial.println("Processing B");
      state = "backward";
      drive(LOW, 100, LOW, 100, 100);
      
      delay(50);
    } else if (cmd == 'R') {
      Serial.println("Processing R");
      state = "right";
      drive(HIGH, 30, LOW, 20, 50);
      
      delay(50);


    } else if (cmd == 'L') {
      Serial.println("Processing L");
      state = "left";
       drive(LOW, 20, HIGH, 30, 50);
      
      delay(50);
    }
    cmdList.remove(0,1);
    Serial.println("List: " + cmdList);
    
  }
}
