#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define CLAW 11
#define CLAW_INIT 329
#define CLAW_SLEEP 330
#define CLAW_CLOSED 465
#define CLAW_OPEN 290

#define WRIST 10
#define WRIST_INIT 339
#define WRIST_SLEEP 340
#define WRIST_LEFT 580
#define WRIST_RIGHT 100
 
#define WRIST_TILT 9
#define WRIST_TILT_INIT 579
#define WRIST_TILT_SLEEP 400
#define WRIST_UP 580
#define WRIST_DOWN 100

#define ELBOW 7
#define ELBOW_INIT 449
#define ELBOW_SLEEP 450
#define ELBOW_STRAIGHT 200
#define ELBOW_BENT 450

#define SHOULDER_TILT 6
#define SHOULDER_TILT_INIT 299
#define SHOULDER_TILT_SLEEP 500
#define SHOULDER_UP 350
#define SHOULDER_DOWN 160

#define SHOULDER 5
#define SHOULDER_INIT 359
#define SHOULDER_SLEEP 120
#define SHOULDER_LEFT 600
#define SHOULDER_RIGHT 120

int testservo = 5;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int currentClaw = CLAW_INIT;
int currentWrist = WRIST_INIT;
int currentWristTilt = WRIST_TILT_INIT;
int currentElbow = ELBOW_INIT;
int currentShoulderTilt = SHOULDER_TILT_INIT;
int currentShoulder = SHOULDER_INIT;
byte increment = 10;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  setAwake();
}

int changePulse(byte servo, int startPulse, int endPulse){
  if(startPulse < endPulse){
    for (uint16_t pulselen = startPulse; pulselen < endPulse; pulselen++) {
      pwm.setPWM(servo, 0, pulselen);
      delay(5);
    }
  }
  if(startPulse > endPulse){
    for (uint16_t pulselen = startPulse; pulselen > endPulse; pulselen--) {
      pwm.setPWM(servo, 0, pulselen);
      delay(5);
    }    
  }
  
  return endPulse;
}

void setSleep(){
  currentClaw = changePulse(CLAW, currentClaw, CLAW_SLEEP);
  currentWrist = changePulse(WRIST, currentWrist, WRIST_SLEEP);
  currentWristTilt = changePulse(WRIST_TILT, currentWristTilt, WRIST_TILT_SLEEP);
  currentElbow = changePulse(ELBOW, currentElbow, ELBOW_SLEEP);
  currentShoulderTilt = changePulse(SHOULDER_TILT, currentShoulderTilt, SHOULDER_TILT_SLEEP);
  currentShoulder = changePulse(SHOULDER, currentShoulder, SHOULDER_SLEEP);    
}

void setAwake(){
  currentClaw = changePulse(CLAW, CLAW_SLEEP, CLAW_INIT);
  currentWrist = changePulse(WRIST, WRIST_SLEEP, WRIST_INIT);
  currentWristTilt = changePulse(WRIST_TILT, WRIST_TILT_SLEEP, WRIST_TILT_INIT);
  currentElbow = changePulse(ELBOW, ELBOW_SLEEP, ELBOW_INIT);
  currentShoulderTilt = changePulse(SHOULDER_TILT, SHOULDER_TILT_SLEEP, SHOULDER_TILT_INIT);
  currentShoulder = changePulse(SHOULDER, SHOULDER_SLEEP, SHOULDER_INIT);  
}

int setReqActionEndPulse(int endPulse, byte action){
  if(action >= 97 && action <= 122){
    endPulse -= increment;
  }
  if(action >= 65 && action <= 90){
    endPulse += increment;
  }
  return endPulse;
}

int setReqEndPulse(int endPulse, int minPulse, int maxPulse){
  if(endPulse > maxPulse){
    endPulse = maxPulse;
  }
  if(endPulse < minPulse){
    endPulse = minPulse;
  }
  return endPulse;
}

void loop() {
  byte tmp = 255;
  byte reqAction = 0;
  int reqEndPulse = 0;
  if(Serial.available()){
    tmp = Serial.read();
    if(tmp == 10){
      return;
    }
    if(tmp == 37)
    {
      setSleep();
      return;
    }
    if(tmp == 38)
    {
      setAwake();
      return;
    }
    Serial.print("setting servo action: ");
    reqAction = tmp;    
    Serial.println(reqAction);
  }
  switch(reqAction){
    case 105:{
      increment -= 5;
      if(increment < 5){
        increment = 5;
      }
      Serial.println(increment);
      break;
    }
    case 73:{
      increment += 5;
      if (increment > 50){
        increment = 50;
      }
      Serial.println(increment);
      break;
    }    
    case 67:
    case 99:{
      Serial.println("CLAW");
      reqEndPulse = setReqActionEndPulse(currentClaw, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, CLAW_OPEN, CLAW_CLOSED);
      currentClaw = changePulse(CLAW, currentClaw, reqEndPulse);
      break;
    } 
    case 87:
    case 119:
    {
      Serial.println("WRIST");
      reqEndPulse = setReqActionEndPulse(currentWrist, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, WRIST_RIGHT, WRIST_LEFT);
      currentWrist = changePulse(WRIST, currentWrist, reqEndPulse);
      break;
    } 
    case 88:
    case 120:
    {
      Serial.println("WRIST_TILT");
      reqEndPulse = setReqActionEndPulse(currentWristTilt, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, WRIST_DOWN, WRIST_UP);
      currentWristTilt = changePulse(WRIST_TILT, currentWristTilt, reqEndPulse);
      break;
    } 
    case 69:
    case 101:{
      Serial.println("ELBOW");
      reqEndPulse = setReqActionEndPulse(currentElbow, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, ELBOW_STRAIGHT, ELBOW_BENT);
      currentElbow = changePulse(ELBOW, currentElbow, reqEndPulse);
      break;
    } 
    case 84:
    case 116:
    {
      Serial.println("SHOULDER_TILT");
      reqEndPulse = setReqActionEndPulse(currentShoulderTilt, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, SHOULDER_DOWN, SHOULDER_UP);
      currentShoulderTilt = changePulse(SHOULDER_TILT, currentShoulderTilt, reqEndPulse);
      break;
    } 
    case 83:
    case 115:
    {
      Serial.println("SHOULDER");
      reqEndPulse = setReqActionEndPulse(currentShoulder, reqAction);
      reqEndPulse = setReqEndPulse(reqEndPulse, SHOULDER_RIGHT, SHOULDER_LEFT);
      currentShoulder = changePulse(SHOULDER, currentShoulder, reqEndPulse);
      break;
    } 
  }
//  delay(10000);
//  setSleep();
//  delay(10000);
//  setAwake();
}
