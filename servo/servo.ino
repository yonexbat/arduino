#include <Servo.h>

Servo servo;
Servo speed;
int servoPos = 1500;
int newServoPos = 1500;

int speedPos = 1580;
int newSpeedPos = 1580;

unsigned long servoLastTime;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  setupServoAndEsc();
}

// Servo and electronic speed controller.
void setupServoAndEsc()
{
  servo.attach(5); 
  speed.attach(3);
  servoLastTime = millis(); 
}

// Setup wifi


// the loop function runs over and over again forever
void loop() {
  int command = readSerial();
  handleCommand(command);
  setServoPosition();
}

void handleCommand(int command)
{
  switch(command)
  {
    case 97:
      digitalWrite(LED_BUILTIN, LOW);
      newServoPos = 1500; 
      break;
    case 98:
      digitalWrite(LED_BUILTIN, HIGH);
      newServoPos = 1000;
      break;
    case 99:
      digitalWrite(LED_BUILTIN, HIGH);
      newServoPos = 1800;
      break;    

    case 100:
      speedPos += 10;
      speed.writeMicroseconds(speedPos);
      break;
    case 101:
      speedPos += -10;
      speed.writeMicroseconds(speedPos);
      break;
  }
}

void setServoPosition()
{  
  unsigned long currentTime = millis();

  if((currentTime - servoLastTime) <= 100) {
    return;
  }

  if(newServoPos == servoPos)
  {
    return;
  }

  int diff = abs(newServoPos - servoPos);

  if(diff > 30)
  {
    if(newServoPos > servoPos)
    {
      servoPos += 30;
    } 
    else if (newServoPos < servoPos) {
      servoPos -= 30;   
    }
  }
  else {
    servoPos = newServoPos;
  }

  // prevent overflow
  if(servoPos > 2000) {
    servoPos = 2000;
  }
  if(servoPos < 1000)
  {
    servoPos = 1000;
  }

  servo.writeMicroseconds(servoPos);
  
  servoLastTime = millis();
}

int readSerial() {
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
    Serial.print("Servo current: ");
    Serial.println(servoPos, DEC);
    Serial.print("Speed current: ");
    Serial.println(speedPos, DEC);
    return incomingByte;
  }
  return 0;  
}
