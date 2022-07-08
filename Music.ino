#include "OneMsTaskTimer.h"

const int buzzer1 = 32; // music buzzer pin
const int buzzer2 = 11; // sound effects

// set up music timer
OneMsTaskTimer_t  timerTask3 = {10, musicISR, 0, 0};
int musicFlag;

// set up states for music state machine
enum MusicStates {BEGIN, MUSICWAIT, BACKGROUND, WIN};
MusicStates musicState;

// tones and delays in order for background music
                          // D      D#      E       D    --- D        E       F      C --- D       C      D     C*  --- A#    A    G      F        C
int backgroundTones[42] = {587.33,622.25,659.25,0,587.33,0,587.33,0,659.25,698.46,1046.5,1180.7,1046.5,1180.7,1046.5,932.33,880,783.99,698.46,0,523.25, //21
                           587.33,622.25,659.25,0,587.33,0,587.33,0,659.25,698.46,1046.5,1396.9,1174.7,1046.5,932.33,880,783.99,698.46,0,523.25,554.37}; // 21
                          // D      D#      E       D    --- D        E       F      C --- F      D       C     A#     A    G     F  ---    C      C#
int backgroundDelays[42] = {250,250,250,125,125,125,125,125,125,250,250,250,250,250,250,125,125,125,125,250,250,250,250,250,125,125,125,125,125,125,
                            250,250,250,125,125,125,125,125,125,750,125,125};
int backCount = 0; // index

// tones and delays in order for win music
int winTones[27] = {196,261.63,329.63,392,523.25,659.25,783.99,659.25,207.65,261.63,311.13,415.3,523.25,622.25,830.61,622.25,233.08,293.66,349.23,
                    466.16,587.33,698.46,932.33,932.33,932.33,932.33,932.33};
int winDelays[27] = {125,125,125,125,125,125,375,375,125,125,125,125,125,125,375,375,125,125,125,125,125,125,375,125,125,750};
int winCount = 0; // index

// flags to determine which music plays
int backgroundFlag;
int winFlag;

void setupMusic() {
  Serial.begin(9600);
  musicState = BEGIN; // set initial state
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  OneMsTaskTimer::add(&timerTask3);
  OneMsTaskTimer::start();
}

void musicFunc() {
  switch(musicState) {
    case BEGIN:
      musicState = MUSICWAIT;
      break;
    case MUSICWAIT:
      if (backgroundFlag == 1) { // play backgroudn music
        musicState = BACKGROUND;
      }
      else if (winFlag == 1) { // play highscore music
        musicState = WIN;
      }
      break;
    case BACKGROUND:
      if (backgroundFlag == 0) { // stop playing music
        musicState = MUSICWAIT;
        backCount = 0;
      }
      break;
    case WIN:
      if (winFlag == 0) { // stop playing music
        noTone(buzzer1);
        musicState = MUSICWAIT;
        winCount = 0;
      }
      break;
  }
  switch(musicState) {
    case BEGIN:
      break;
    case MUSICWAIT:
      break;
    case BACKGROUND:
      backgroundMusic(); // play music
      delay(backgroundDelays[backCount-1]); // length of each note/rest
      break;
    case WIN:
      winMusic(); // play music
      delay(winDelays[winCount-1]); // length of each note/rest
      break;
  }
}

void loopMusic() {
  while(musicFlag == 0) {delay(5);}
  musicFlag = 0;
  musicFunc();
  delay(5);
}

void musicISR() {
  musicFlag = 1;
}

void backgroundMusic() {
  if (backCount == 42) {
    backCount = 0; // reset index to 0 when it reaches end of song
  }
  if (backgroundTones[backCount] == 0) {
    noTone(buzzer1); // rest
  }
  else {
    tone(buzzer1, backgroundTones[backCount]); // play note
  }
  backCount++; // move to next note
}

void winMusic() {
  if (winCount == 27) {
    winCount = 0;
    winFlag = 0; // stop playing (only plays once)
  }
  if (winTones[winCount] == 0) {
    noTone(buzzer1); // rest
  }
  else {
    tone(buzzer1, winTones[winCount]); // play note
  }
  winCount++; // move to next note
}
