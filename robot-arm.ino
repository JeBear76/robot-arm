#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define CLAW 11
#define CLAW_INIT 329
#define CLAW_SLEEP 330
#define CLAW_MAX 465    // Closed
#define CLAW_MIN 290    // Open

#define WRIST 10
#define WRIST_INIT 339
#define WRIST_SLEEP 340
#define WRIST_MAX 580
#define WRIST_MIN 100
 
#define WRIST_TILT 9
#define WRIST_TILT_INIT 579
#define WRIST_TILT_SLEEP 250
#define WRIST_TILT_MAX 580
#define WRIST_TILT_MIN 100

#define ELBOW 7
#define ELBOW_INIT 449
#define ELBOW_SLEEP 450
#define ELBOW_MAX 450  // Bent
#define ELBOW_MIN 200  // Straight

#define SHOULDER_TILT 6
#define SHOULDER_TILT_INIT 299
#define SHOULDER_TILT_SLEEP 500
#define SHOULDER_TILT_MAX 350
#define SHOULDER_TILT_MIN 160

#define SHOULDER 5
#define SHOULDER_INIT 359
#define SHOULDER_SLEEP 120
#define SHOULDER_MAX 600
#define SHOULDER_MIN 120

bool inSleepPosition = true;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int currentClaw = CLAW_INIT;
int currentWrist = WRIST_INIT;
int currentWristTilt = WRIST_TILT_INIT;
int currentElbow = ELBOW_INIT;
int currentShoulderTilt = SHOULDER_TILT_INIT;
int currentShoulder = SHOULDER_INIT;
byte increment = 10;    // Default increment to avoid breaking things on first use

void setup() {
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);
  setAwake();
}

/*
 * Activates teh appropriate servo
 */
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

/*
 * Moves teh arm to its sleep position
 */
void setSleep(){
  currentShoulderTilt = changePulse(SHOULDER_TILT, currentShoulderTilt, SHOULDER_TILT_MAX);
  currentClaw = changePulse(CLAW, currentClaw, CLAW_SLEEP);
  currentWrist = changePulse(WRIST, currentWrist, WRIST_SLEEP);
  currentWristTilt = changePulse(WRIST_TILT, currentWristTilt, WRIST_TILT_INIT);
  currentElbow = changePulse(ELBOW, currentElbow, ELBOW_SLEEP);
  currentShoulder = changePulse(SHOULDER, currentShoulder, SHOULDER_SLEEP);
  currentShoulderTilt = changePulse(SHOULDER_TILT, currentShoulderTilt, SHOULDER_TILT_SLEEP);
  currentWristTilt = changePulse(WRIST_TILT, currentWristTilt, WRIST_TILT_SLEEP);
}

/*
 * Moves teh arm to its start position
 */
void setAwake(){
  // Will do nothing if the arm isn't in sleep position
  // This is to prevent it jerking around and damaging itself
  if(!inSleepPosition){
    return;
  }
  inSleepPosition = false;
  currentClaw = changePulse(CLAW, CLAW_SLEEP, CLAW_INIT);
  currentWrist = changePulse(WRIST, WRIST_SLEEP, WRIST_INIT);
  currentWristTilt = changePulse(WRIST_TILT, WRIST_TILT_SLEEP, WRIST_TILT_INIT);
  currentElbow = changePulse(ELBOW, ELBOW_SLEEP, ELBOW_INIT);
  currentShoulderTilt = changePulse(SHOULDER_TILT, SHOULDER_TILT_SLEEP, SHOULDER_TILT_INIT);
  currentShoulder = changePulse(SHOULDER, SHOULDER_SLEEP, SHOULDER_INIT);  
}

/*
 * Sets the endPulse for the movement based on action
 */
int setReqActionEndPulse(int endPulse, byte action){
  if(action >= 97 && action <= 122){
    endPulse -= increment;
  }
  if(action >= 65 && action <= 90){
    endPulse += increment;
  }
  return endPulse;
}

/*
 * Keeps the endPulse for the movement within the boundaries of the servo
 */
int validateReqEndPulse(int endPulse, int minPulse, int maxPulse){
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
    if(tmp == 37) // %
    {
      setSleep(); 
      return;
    }
    if(tmp == 38) // &
    {
      setAwake();
      return;
    }
    Serial.print("setting servo action: ");
    reqAction = tmp;    
    Serial.println(reqAction);
    if(reqAction >= 48 && reqAction <= 57)  // Digits 0 to 9
    {
      increment = (reqAction - '0' + 1) * 5;
      Serial.println(increment);
    }
    // One letter per servo. Uppercase for increment and lowercase for decrement
    switch(reqAction){
      case 67:  // C
      case 99:  // c
      {
//        Serial.println("CLAW");
        reqEndPulse = setReqActionEndPulse(currentClaw, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, CLAW_MIN, CLAW_MAX);
        currentClaw = changePulse(CLAW, currentClaw, reqEndPulse);
        break;
      } 
      case 87:  // W
      case 119: // w
      {
//        Serial.println("WRIST");
        reqEndPulse = setReqActionEndPulse(currentWrist, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, WRIST_MIN, WRIST_MAX);
        currentWrist = changePulse(WRIST, currentWrist, reqEndPulse);
        break;
      } 
      case 88:  // X
      case 120: // x
      {
//        Serial.println("WRIST_TILT");
        reqEndPulse = setReqActionEndPulse(currentWristTilt, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, WRIST_TILT_MIN, WRIST_TILT_MAX);
        currentWristTilt = changePulse(WRIST_TILT, currentWristTilt, reqEndPulse);
        break;
      } 
      case 69:  //E
      case 101: //e
      {
//        Serial.println("ELBOW");
        reqEndPulse = setReqActionEndPulse(currentElbow, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, ELBOW_MIN, ELBOW_MAX);
        currentElbow = changePulse(ELBOW, currentElbow, reqEndPulse);
        break;
      } 
      case 84:  // T
      case 116: // t
      {
//        Serial.println("SHOULDER_TILT");
        reqEndPulse = setReqActionEndPulse(currentShoulderTilt, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, SHOULDER_TILT_MIN, SHOULDER_TILT_MAX);
        currentShoulderTilt = changePulse(SHOULDER_TILT, currentShoulderTilt, reqEndPulse);
        break;
      } 
      case 83:  // S
      case 115: // s
      {
//        Serial.println("SHOULDER");
        reqEndPulse = setReqActionEndPulse(currentShoulder, reqAction);
        reqEndPulse = validateReqEndPulse(reqEndPulse, SHOULDER_MIN, SHOULDER_MAX);
        currentShoulder = changePulse(SHOULDER, currentShoulder, reqEndPulse);
        break;
      } 
    }  
  }
}
