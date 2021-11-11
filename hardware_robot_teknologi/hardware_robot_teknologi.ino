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
 
  if(stateButton == "Start"){
    processCmd();
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
  analogWrite(motorLeftSpeed, leftSpeed);
  
  //Motor B (Right)
  digitalWrite(motorRightDirection, rightDirection);
  analogWrite(motorRightSpeed, rightSpeed);

  //Driving distance/time
  delay(distance);

  //stop
  analogWrite(motorRightSpeed, 0);
  analogWrite(motorLeftSpeed, 0);
  
}

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
    char cmd = cmdList.charAt(0);
    cmdList.remove(0,1);
    Serial.println("List: " + cmdList);
    
    if(cmd == 'F') {
      
      Serial.println("Processing F");
      drive(HIGH, 255, HIGH, 255, 1000);
      
    } else if (cmd == 'B') {
      
      Serial.println("Processing B");
      drive(LOW, 255, LOW, 255, 1000);
      
    } else if (cmd == 'R') {
      
      Serial.println("Processing R");
      drive(HIGH, 200, LOW, 150, 1000);
      
    } else if (cmd == 'L') {
      
      Serial.println("Processing L");
      drive(LOW, 150, HIGH, 200, 1000);
      
    }
    
    
    
  }
}
