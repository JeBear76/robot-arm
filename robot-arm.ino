#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define CLAW 11
#define CLAW_SLEEP 325
#define CLAW_INIT 325
#define CLAW_CLOSED 465
#define CLAW_OPEN 290

#define WRIST 10
#define WRIST_INIT 340
#define WRIST_SLEEP 340
#define WRIST_LEFT 580
#define WRIST_RIGHT 100
 
#define WRIST_TILT 9
#define WRIST_TILT_INIT 580
#define WRIST_TILT_SLEEP 400
#define WRIST_UP 580
#define WRIST_DOWN 100

#define ELBOW 7
#define ELBOW_INIT 450
#define ELBOW_SLEEP 450
#define ELBOW_STRAIGHT 200
#define ELBOW_BENT 450

#define SHOULDER_TILT 6
#define SHOULDER_TILT_INIT 300
#define SHOULDER_TILT_SLEEP 500
#define SHOULDER_UP 350
#define SHOULDER_DOWN 160

#define SHOULDER 5
#define SHOULDER_INIT 360
#define SHOULDER_SLEEP 360
#define SHOULDER_LEFT 600
#define SHOULDER_RIGHT 120

int testservo = 5;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

int currentClaw = 325;
int currentWrist = 340;
int currentWristTilt = 580;
int currentElbow = 450;
int currentShoulderTilt = 300;
int currentShoulder = 360;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pwm.begin();
  pwm.setPWMFreq(60);
  delay(10);

  setAwake();
}

int changePulse(int servo, int startPulse, int endPulse){
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

void loop() {
  delay(10000);
  setSleep();
  delay(10000);
  setAwake();
}
