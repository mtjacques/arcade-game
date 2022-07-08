#include "OneMsTaskTimer.h"

// initialize pins and flags
int selectPin = 31;
int joystickPin = 30;
int jumpFlag = 0;
int timerFlag = 0;
int forwardFlag = 0;
int backwardFlag = 0;

// thresholds for joystick
int leftThreshold = 350;
int rightThreshold = 600;

// Create timer for hero
OneMsTaskTimer_t  timerTask = {130, playerActionTimerISR, 0, 0}; //standard is 130

// define struct for all objects in game
typedef struct xy_struct {
  int x;
  int y;
} XY;

// create hero and coin objects
XY HeroLocation;
XY prevHeroLocation;
XY coin;

// Create states
enum PlayerStates {START, INIT, WAITING, FORWARD, BACKWARD, JUMP};
PlayerStates state;

// determines how long a jump lasts
int jumpCnt;

void setupPlayerActions() {
  state = START;
  Serial.begin(9600);
  pinMode(selectPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(selectPin), jumpISR, RISING); // set jump interrupt pin
  OneMsTaskTimer::add(&timerTask);
  OneMsTaskTimer::start();
}

void tickFunc() {
  //State Transitions
  switch(state) {
    case START:
      //Serial.println("Transition: Start to Waiting");
      state = INIT;
      break;
    case INIT:
      //Serial.println("Transition: Init to Waiting");
      state = WAITING;
      break;
    case WAITING:
      if (forwardFlag == 1) {
        forwardFlag = 0;
        //Serial.println("Transition: Waiting to Forward");
        state = FORWARD;
      }
      else if (backwardFlag == 1) {
        backwardFlag = 0;
        //Serial.println("Transition: Waiting to Backward");
        state = BACKWARD;
      }
      else if (jumpFlag == 1) {
        jumpCnt = 0;
        state = JUMP;
        //Serial.println("Transition: Waiting to Jump");
      }
      break;
    case FORWARD:
      //Serial.println("Transition: Forward to Waiting");
      state = WAITING;
      break;
    case BACKWARD:
      //Serial.println("Transition: Backward to Waiting");
      state = WAITING;
      break;
    case JUMP:
      if (jumpCnt < 6) { // three refresh ticks
        //Serial.println("Transition: Jump to Jump");
          if (jumpCnt == 1 && HeroLocation.x != coin.x) {
            jumpSound();
          }
        state = JUMP;
      }
      else if (jumpCnt == 6) { // three refresh ticks
        //Serial.println("Transition: Jump to Waiting");
        HeroLocation.y = 1;
        jumpFlag = 0;
        state = WAITING;
      }
      break;
  }

  switch(state) {
    case START:
      break;
    case INIT:
      // initialize hero location
      HeroLocation.x = 0;
      HeroLocation.y = 1;
      break;
    case WAITING:
      /*Serial.print("Hero Location: ");
      Serial.print(HeroLocation.x);
      Serial.print(" ");
      Serial.println(HeroLocation.y);*/
      break;
    case FORWARD:
      prevHeroLocation = HeroLocation;
      (HeroLocation.x)++; // move hero forward
      break;
    case BACKWARD:
      prevHeroLocation = HeroLocation;
      (HeroLocation.x)--; // move hero backward
      break;
    case JUMP:
      HeroLocation.y = 0;
      /*Serial.print("Hero Location: ");
      Serial.print(HeroLocation.x);
      Serial.print(" ");
      Serial.println(HeroLocation.y);*/
      jumpCnt++;
      break;
  }
}

void loopPlayerActions() {
  while(timerFlag == 0) {delay(10);}
  //Serial.println(analogRead(joystickPin));
  timerFlag = 0;
  tickFunc();
  delay(10);
}

void jumpISR() {
  //Serial.println("Jump Detected!");
  jumpFlag = 1;
}

void playerActionTimerISR() {
  timerFlag = 1;
  // check joystick state to see if hero needs to move
  if (analogRead(joystickPin) < leftThreshold) {
    backwardFlag = 1;
  }
  else if (analogRead(joystickPin) > rightThreshold) { // CHECK
    forwardFlag = 1;
  }
}

void jumpSound() {
  noTone(buzzer1); // make sure background music stops while jump sound plays
  tone(buzzer2, 130.81);
  delay(30);
  tone(buzzer2, 164.81);
  delay(30);
  tone(buzzer2, 196);
  delay(30);
  tone(buzzer2, 261.63);
  delay(30);
  tone(buzzer2, 329.63);
  delay(30);
  tone(buzzer2, 392);
  delay(30);
  tone(buzzer2, 523.25);
  delay(30);
  noTone(buzzer2);
}
