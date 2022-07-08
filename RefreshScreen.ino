byte hero[8] = { // Hero Design
  B00000,
  B00110,
  B00110,
  B00000,
  B01110,
  B00111,
  B00110,
  B00101,
};

// Create states to refresh screen
enum RefreshStates {REFRESHSTART, DRAW, OVER};
RefreshStates refreshState;

// Set up screen timer
OneMsTaskTimer_t  timerTask2 = {250, screenTimerISR, 0, 0}; // standard is 250
int timerFlag2= 0;

// score/highscore variables
int score;
int highscore = 0;
char highName[2];
bool high = false;
char initials[26] = {'A','B','C','D','E','F','G','H','I',
                     'J','K','L','M','N','O','P','Q','R',
                     'S','T','U','V','W','X','Y','Z'};
char name[2];

// obstacle objects
XY obstacle;
XY prevObstacle;

void setupRefreshScreen() {
  lcd.createChar(0, hero); //set up hero character
  refreshState = REFRESHSTART; // set state to start
  OneMsTaskTimer::add(&timerTask2);
  OneMsTaskTimer::start();
  backgroundFlag = 0;
  winFlag = 0;
}

void drawFunc() {
  switch(refreshState) { // transition cases
    case REFRESHSTART:
      break;
    case DRAW:
      // determine if a collision has happened
      if (prevHeroLocation.x == obstacle.x && HeroLocation.y == prevObstacle.y) {
        refreshState = OVER;
      }
      else if (prevHeroLocation.x == (obstacle.x + 1) && HeroLocation.y == prevObstacle.y) {
        refreshState = OVER;
      }
      else {
        if (prevHeroLocation.x == prevObstacle.x && prevHeroLocation.y != prevObstacle.y && HeroLocation.x != obstacle.x) {
          score++; // increase score if an obstacle is avoided 
        }
        else if (HeroLocation.x == coin.x && HeroLocation.y == coin.y) {
          coinSound();
          score = score + 2; // add two to score if coin is collected
          getNewCoinLocation();
        }
        refreshState = DRAW;
      }
      // For Testing Purposes
      /*Serial.print("Location: ");
      Serial.print(prevHeroLocation.x);
      Serial.print(" ");
      Serial.print(HeroLocation.y);
      Serial.print(" | ");
      Serial.print(obstacle.x);
      Serial.print(" ");
      Serial.println(prevObstacle.y);*/
      break;
  }

  switch(refreshState) {
    case REFRESHSTART:
      getInitials(); // get players initial first
      obstacle.x = 15;
      obstacle.y = 1;
      score = 0; // initialize score
      refreshState = DRAW;
      getNewCoinLocation(); // get first coin location
      drawCoin(coin.x, coin.y); // draw first coin
      break;
    case DRAW:
      // draw everything on screen
      backgroundFlag = 1;
      drawHero();
      drawObstacle();
      drawCoin(coin.x, coin.y);
      break;
    case OVER:
      backgroundFlag = 0;
      displayGameOver();
      break;
  }
}

void loopRefreshScreen() {
  while(timerFlag2 == 0) {delay(10);}
  timerFlag2 = 0;
  lcd.clear();
  drawFunc();
  delay(10);
}

void getInitials() {
  // Display instructions
  lcd.setCursor(0,0);
  lcd.print("Enter First");
  lcd.setCursor(0,1);
  lcd.print("Initial:");
  delay(2000);
  lcd.clear();

  // Get first letter from joystick
  int index = 0;
  while(jumpFlag != 1) {
    lcd.setCursor(0,0);
    lcd.print(initials[index]);
    if(forwardFlag == 1 && index != 25) {
      index++;
      forwardFlag = 0;
    }
    if (backwardFlag == 1 && index != 0) {
      index--;
      backwardFlag = 0;
    }
    delay(100);
    lcd.clear();
  }

  // set first initial and display second instructions
  name[0] = initials[index];
  lcd.setCursor(0,0);
  lcd.print("Enter Second");
  lcd.setCursor(0,1);
  lcd.print("Initial:");
  delay(2000);

  // get second letter from joystick
  index = 0;
  while(jumpFlag != 1) {
    lcd.setCursor(0,0);
    lcd.print(initials[index]);
    if(forwardFlag == 1 && index != 25) {
      index++;
      forwardFlag = 0;
    }
    if (backwardFlag == 1 && index != 0) {
      index--;
      backwardFlag = 0;
    }
    delay(250);
    lcd.clear();
  }
  // set second initial
  name[1] = initials[index];

  // make sure hero is in correct place at start of game
  HeroLocation.x = 0;
  HeroLocation.y = 1;
}

void screenTimerISR() { // ticks to refresh the screen
  timerFlag2 = 1;
}

void eraseHero() {
  lcd.setCursor(prevHeroLocation.x, prevHeroLocation.y);
  lcd.print(" ");
}

void drawHero() {
  eraseHero();
  // make sure hero isn't outside the bounds of the lcd
  if (HeroLocation.x < 0) {
    HeroLocation.x = 0;
  }
  if (HeroLocation.x > 15) {
    HeroLocation.x = 15;
  }
  lcd.setCursor(HeroLocation.x, HeroLocation.y);
  lcd.write(byte(0));
  prevHeroLocation = HeroLocation;
}

void drawObstacle() {
  eraseObstacle();
  lcd.setCursor(obstacle.x, obstacle.y);
  lcd.print("|");
  prevObstacle = obstacle;
  (obstacle.x)--; // move hero left one box every clock tick
  if (obstacle.x < 0) {
    obstacle.x = 15;
  }
}
void eraseObstacle() {
  lcd.setCursor(prevObstacle.x, prevObstacle.y);
  lcd.print(" ");
}

void drawCoin(int cx, int cy) {
  lcd.setCursor(cx, cy);
  lcd.print("*");
}

void getNewCoinLocation() {
  lcd.setCursor(coin.x, coin.y);
  lcd.print(" ");
  // get random x and y value for coin
  int cx = rand() % 16;
  int cy = rand() % 2;
  while (cx == obstacle.x || cx == prevObstacle.x || cx == HeroLocation.x) {
    cx = rand() % 16;
  }
  coin.x = cx;
  coin.y = cy;
}

void coinSound() {
  noTone(buzzer1); // make sure background music stops
  tone(buzzer2, 987.77);
  delay(75);
  tone(buzzer2, 1318.5);
  delay(75);
  noTone(buzzer2);
}

void gameOverSound() {
  noTone(buzzer1); // make sure background music stops
  tone(buzzer2, 196);
  delay(200);
  tone(buzzer2, 130.81);
  delay(500);
  noTone(buzzer2);
}

void checkHighscore() {
  if (score > highscore) {
    highscore = score; // if true set new highscore
    for (int i = 0; i < 2; i++) {
      highName[i] = name[i]; // set new initials
    }
    high = true;
  }
}

void displayGameOver() {
  //print game over information
  eraseObstacle();
  eraseHero();
  lcd.setCursor(0,0);
  lcd.print("GAME OVER! ");
  gameOverSound();
  delay(1500);
  lcd.clear();

  // check if new highscore occured
  checkHighscore();
  delay(50);
  
    if (high == true) { // new highscore happened
      winFlag = 1; // plays win music in Music tab
      lcd.setCursor(0,0);
      lcd.print("NEW HIGH SCORE!");
      delay(3000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(name);
      lcd.print(" HIGHSCORE: ");
      lcd.setCursor(0,1);
      lcd.print(score);
      delay(5000);
    }
    else { // not a highscore
      lcd.setCursor(0,0);
      lcd.print(name);
      lcd.print(" SCORE: ");
      lcd.print(score);
      delay(5000);
      lcd.setCursor(0,0);
      lcd.print("HIGHSCORE: ");
      lcd.print(highName);
      lcd.setCursor(0,1);
      lcd.print(highscore);
      delay(5000);
    }
    lcd.clear();
    high = false; // make sure highscore is false for next runthrough

    // use button to restart the game
    while(jumpFlag != 1) {
      lcd.setCursor(0,0);
      lcd.print("PUSH TO RESTART");
      delay(100);
    }
  lcd.clear();
  // reset game
  refreshState = REFRESHSTART;
  state = START;
  musicState = BEGIN;
  jumpFlag = 0;
}
