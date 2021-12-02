#include <EEPROM.h>

//Motor A (Left)
int motorLeftDirection = 12;   //DIR A
int motorLeftSpeed = 6;        //PWM A

//Motor B (Right)
int motorRightDirection = 13;   //DIR B
int motorRightSpeed = 11;       //PWM B

String cmdList = "";

String stateButton = "Stop";

// Shift-Register
int latchPin = 5; //ST_CP or RCLK
int clockPin = 4; //SH_CP or SRCLK
int dataPin = 7; //DS or SER

// Distance-sensor
int trigPin = 9;            //the used trig pin
int echoPin = 8;            //the used echo pin
//int distance; //Distance of elements in front of robot

// Line-sensor
int leftLineSensor= A5;
int rightLineSensor = A4;

int analogLeft = 0;
int analogRight = 0;

void setup() {


  
  // multiplexer
  /*
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(comOutInPin, INPUT_PULLUP);
  */
  
  //Motor A (Left)
  pinMode(motorLeftDirection, OUTPUT);
  pinMode(motorLeftSpeed, OUTPUT);
  
  //Motor B (Right)
  pinMode(motorRightDirection, OUTPUT);
  pinMode(motorRightSpeed, OUTPUT);
  
  Serial.begin(9600);

  // Shift-register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);

  pinMode(leftLineSensor, INPUT);
  pinMode(rightLineSensor, INPUT);
  
  //EEPROM
  cmdList = readFromEEPROM();

  writeShiftRegister(B00000000);

}

void loop() {


 
  /*
  Serial.print("Left : ");
  Serial.println(analogRead(leftLineSensor));

  Serial.print("Right : ");
  Serial.println(analogRead(rightLineSensor));
  
*/  
  analogLeft = analogRead(leftLineSensor);
  analogRight = analogRead(rightLineSensor);


  

  if(analogLeft > 700 && analogRight > 700){
     drive(HIGH, 0, HIGH, 0, 500);
     Serial.println("STOPPED");
     
  } else if( analogLeft < 700){
                  drive(HIGH, 200, LOW, 150, 100);

        Serial.println("Left");

  } else if ( analogRight < 700) {
      drive(HIGH, 150, LOW, 200, 100);

      Serial.println("Right");
  }
  

  
  //Serial.println("Left: " + analogRead(leftLineSensor) + " | Right: " + analogRead(rightLineSensor));
  // MULTIPLEXER Checks CMD-buttons
}

void goRight(){
      drive(HIGH, 200, LOW, 150, 500);
}
void goLeft(){
      drive(HIGH, 150, LOW, 200, 500);

}


// Use method to calculate distance, and dictacte if robot should stop for obstacle
int getDistance() {
  //sends out a trigger sound
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  //returns the received echo in centimeter
  return pulseIn(echoPin, HIGH)* 0.034/2;
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
  /*
   for (int i = 0 ; i < 1000 ; i++) {
    delay(distance/1000);
    }
    */

  delay(distance);
  //stop
  analogWrite(motorRightSpeed, 0);
  analogWrite(motorLeftSpeed, 0);
  
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

/*
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
  writeShiftRegister(B00000000);
  if(cmdList=="") {
    changeState();
  }
  Serial.println("Cmdlist saved in eeprom: " +readFromEEPROM());
}*/
