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

String stateButton = "Stop";
String pauseButton = "resume";

// Shift-Register
int latchPin = 4; //ST_CP or RCLK
int clockPin = 5; //SH_CP or SRCLK
int dataPin = 7; //DS or SER

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

  // Shift-register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);

  //EEPROM
  cmdList = readFromEEPROM();
  Serial.println("LAST CMD : " + cmdList);

  writeShiftRegister(B00000000);
}

void loop() {

  // MULTIPLEXER Checks CMD-buttons
  if(stateButton == "Start"){
    if(pauseButton == "resume"){
      
      processCmd();
      
      readStateButton();
      readPauseButton();

    } else if(pauseButton == "pause"){
      readPauseButton();
      readStateButton();
    }

    } else if (stateButton == "Stop"){
        //Serial.println("Reading input from buttons...");
        readCmdButtons();
        readStateButton();
        writeToEEPROM(cmdList);   
    }
}

void writeToEEPROM(String str) {
  int len = str.length();
  EEPROM.write(0, len);

  for (int i = 0; i < len ; i++) {
    EEPROM.write(0+1+i, str[i]);
  }
}

String readFromEEPROM() {
  int len = EEPROM.read(0);

  char data[len +1];
  for (int i = 0 ; i < len ; i++) {
    data[i] = EEPROM.read(0 + 1 + i);
  }
  data[len] = '\0';
  return String(data);
}

void writeShiftRegister(int output) {
  //Bring Latch Pin LOW - prepare to commit the register
  digitalWrite(latchPin, LOW);

  //Overwrites the entire register
  shiftOut(dataPin, clockPin, MSBFIRST, output);

  //Bring Latch Pin HIGH - commits the register
  digitalWrite(latchPin, HIGH);
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
  for (int i = 0 ; i < 1000 ; i++) {
    delay(distance/1000);
    if(readComOutIn(HIGH, HIGH ,LOW)){
      analogWrite(motorRightSpeed, 0);
      analogWrite(motorLeftSpeed, 0);
      changeState();
    } else if (readComOutIn(HIGH, LOW ,LOW)) {
      analogWrite(motorRightSpeed, 0);
      analogWrite(motorLeftSpeed, 0);
      changePauseButton();
    }
  }

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
  if(stateButton == "Stop"){
    stateButton = "Start";
  } else if (stateButton == "Start") {
    stateButton = "Stop";
    cmdList = "";
  }
  return stateButton;
}

String changePauseButton(){
  if(pauseButton == "pause"){
    pauseButton = "resume";
  } else if(pauseButton == "resume"){
    pauseButton = "pause";
  }
  return pauseButton;
}


bool readComOutIn(bool C, bool B, bool A) {
  //Defines which channel the comOutIn port is connected to
  digitalWrite(pinA, A);
  digitalWrite(pinB, B);
  digitalWrite(pinC, C);

  //Reads the channel
  boolean channelValue = digitalRead(comOutInPin);
  if (channelValue) {
    writeShiftRegister(B00000001);  
    delay(50);
    writeShiftRegister(B00000000);
  }
  return channelValue;
}

void readStateButton(){
  if(readComOutIn(HIGH, HIGH, LOW)){
      changeState();
      Serial.println(stateButton);
      delay(100);
      
     while(readComOutIn(HIGH, HIGH, LOW));
    }
}

void readPauseButton(){
    if (readComOutIn(HIGH, LOW ,LOW)) {
        changePauseButton();
        Serial.println(pauseButton);
        delay(100);

        while(readComOutIn(HIGH, LOW ,LOW));
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
    }
}



void processCmd() {
  for (int i = 0 ; i < cmdList.length() ; i++) {
    char cmd = cmdList.charAt(0);
    cmdList.remove(0,1);
    Serial.println("List: " + cmdList);
    
    if(cmd == 'F') {
      Serial.println("Processing F");
      writeShiftRegister(B00010000);
      drive(HIGH, 255, HIGH, 255, 1000);
      //writeShiftRegister(B00000000);
      
    } else if (cmd == 'B') {

      Serial.println("Processing B");
      writeShiftRegister(B00100000);
      drive(LOW, 255, LOW, 255, 1000);
      //writeShiftRegister(B00000000);
      
    } else if (cmd == 'R') {
      
      Serial.println("Processing R");
      writeShiftRegister(B01000000);
      drive(HIGH, 200, LOW, 150, 1000);
      //writeShiftRegister(B00000000);
      
    } else if (cmd == 'L') {

      Serial.println("Processing L");
      writeShiftRegister(B10000000);
      drive(LOW, 150, HIGH, 200, 1000);
      //writeShiftRegister(B00000000);
      
    }
    readPauseButton();
  }
 // writeShiftRegister(B00000000);
  Serial.println("Cmdlist saved in eeprom: " +readFromEEPROM());
}
