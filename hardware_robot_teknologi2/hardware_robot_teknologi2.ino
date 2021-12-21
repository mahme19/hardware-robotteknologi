
//Motor A (Left)
int motorLeftDirection = 12;   //DIR A
int motorLeftSpeed = 6;        //PWM A

//Motor B (Right)
int motorRightDirection = 13;   //DIR B
int motorRightSpeed = 11;       //PWM B

// Shift-Register
int latchPin = 5; //ST_CP or RCLK
int clockPin = 4; //SH_CP or SRCLK
int dataPin = 7; //DS or SER

// Distance-sensor
int trigPin = 9;            //the used trig pin
int echoPin = 8;            //the used echo pin


// Line-sensor
int leftLineSensor= A5;
int rightLineSensor = A4;

int analogLeft = 0;
int analogRight = 0;
int buttonPin = 2;

bool buttonState = digitalRead(buttonPin);

String cmdList = "RLLSSL"; //Optimal path from G to R



void setup() {

  // button
  pinMode(buttonPin, INPUT);
  
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

  //Sensors
  pinMode(leftLineSensor, INPUT);
  pinMode(rightLineSensor, INPUT);

  writeShiftRegister(B00000000);

}


void loop() {
  processCmd();
}


void followLine() {

  while(true) {

    // LEFT DC
    if(analogRead(leftLineSensor) < 800 && getDistance() > 30){
      //Motor A (Left - drive)
      digitalWrite(motorLeftDirection, HIGH);
      analogWrite(motorLeftSpeed, 180);
      //Serial.println("Left");
    }
    else {
      //Motor A (Left - stop)
      digitalWrite(motorLeftDirection, HIGH);
      analogWrite(motorLeftSpeed, 0);
      delay(10);
    }

    // RIGHT DC
    if(analogRead(rightLineSensor) < 800 && getDistance() > 30){
      //Motor B (Right - drive)
      digitalWrite(motorRightDirection, LOW);
      analogWrite(motorRightSpeed, 180);
    }
    else {
      //Motor B (Right - stop)
      digitalWrite(motorRightDirection, LOW);
      analogWrite(motorRightSpeed, 0);
      delay(10);
    }

    if(analogRead(leftLineSensor) > 800 &&  analogRead(rightLineSensor) > 800){
  
      break;
    }
  }  
}

void turnRight(){
  //Bakker for at opnå optimalt udgangspunkt (VIGTIGT)
  drive(LOW, 150, HIGH, 150, 10);
  
  Serial.println("Turning right");
  while(true) {
    digitalWrite(motorLeftDirection, HIGH);
    analogWrite(motorLeftSpeed, 180);
    if(analogRead(leftLineSensor) < 700) { //If hits white
      break;  
    }
  }

  while(true) {
    digitalWrite(motorLeftDirection, HIGH);
    analogWrite(motorLeftSpeed, 180);
    if(analogRead(leftLineSensor) > 700) { //If hits black
      break;
    }
  }
}

void turnLeft() {
  //Bakker for at opnå optimalt udgangspunkt (VIGTIGT)
  drive(LOW, 150, HIGH, 150, 10);
  
  Serial.println("Turning left");
  while(true) {
    digitalWrite(motorRightDirection, LOW);
    analogWrite(motorRightSpeed, 180);
    if(analogRead(rightLineSensor) < 700) { //If hits white
      break;  
    }
  }

  while(true) {
    digitalWrite(motorRightDirection, LOW);
    analogWrite(motorRightSpeed, 180);
    if(analogRead(rightLineSensor) > 700) { //If hits black
      break;
    }
  }
}

void continueStraight() {
  drive(HIGH, 150, LOW, 150, 500);
}

void uTurn() {
  //Bakker for at opnå optimalt udgangspunkt (VIGTIGT)
  drive(LOW, 150, HIGH, 150, 10);
  
  Serial.println("U Turning");
  while(true) {
    digitalWrite(motorLeftDirection, HIGH);
    analogWrite(motorLeftSpeed, 150);
    digitalWrite(motorRightDirection, HIGH);
    analogWrite(motorRightSpeed, 150);
    if(analogRead(rightLineSensor) < 700) { //If hits white
      break;  
    }
  }

  while(true) {
    digitalWrite(motorLeftDirection, HIGH);
    analogWrite(motorLeftSpeed, 150);
    digitalWrite(motorRightDirection, HIGH);
    analogWrite(motorRightSpeed, 150);
    if(analogRead(rightLineSensor) > 700) { //If hits black
      break;
    }
  }

  while(true) {
    digitalWrite(motorLeftDirection, HIGH);
    analogWrite(motorLeftSpeed, 150);
    digitalWrite(motorRightDirection, HIGH);
    analogWrite(motorRightSpeed, 150);
    if(analogRead(rightLineSensor) < 700) { //If hits white
      break;  
    }
  }
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
  delay(distance);
  
  //stop
  analogWrite(motorRightSpeed, 0);
  analogWrite(motorLeftSpeed, 0);
  
}

void lightNxtCmd(char nextCmd) {
  if(nextCmd == 'S') {
    Serial.println("S-LIGHT");
    writeShiftRegister(B10000000);
  
  } else if(nextCmd == 'R') {
    Serial.println("R-LIGHT");
    writeShiftRegister(B01000000);
  
  } else if(nextCmd == 'L') {
    Serial.println("L-LIGHT");
    writeShiftRegister(B00100000);
  
  } else if(nextCmd == 'U') {
    Serial.println("U-LIGHT");
    writeShiftRegister(B00010000);
  } 
}

void processCmd() {
  for (int i = 0 ; i < cmdList.length() ; i++) {
    char cmd = cmdList.charAt(i);

    lightNxtCmd(cmd);

    followLine();

    if (cmd == 'S') {
      
      Serial.println("Continuing straight");
      continueStraight();
      
    } else if (cmd == 'R') {

      Serial.println("Going right");
      turnRight();
      
    } else if (cmd == 'L') {
      
      Serial.println("Going left");
      turnLeft();
      
    } else if (cmd == 'U') {

      Serial.println("Going U");
      uTurn();
    
    }

    if(i == cmdList.length()-1){
      writeShiftRegister(B11110000); // Lights up when cmdList empty
      followLine();
      while(true) {
        drive(LOW, 0, HIGH, 0, 0); 
      }
    } 
  }
}
