// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(P6_7, P2_3, P2_6, P2_4, P5_6, P6_6);

void setup() {
  lcd.begin(16, 2);
}

void loop() {
}
