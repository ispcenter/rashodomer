#include <LiquidCrystal_I2C.h>

byte const tonePin = 8;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // put your setup code here, to run once:
  lcd.begin();
  lcd.print("Hello");
  delay(2000);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (analogRead(0) > 2000) {
    lcd.print("Nothing");
    delay(500);
    lcd.clear();
  }
  if (analogRead(0) < 100) {
    lcd.print("Right");
    tone();
    delay(500);
    lcd.clear();
  }
  if (analogRead(0) > 900 & analogRead(0) < 1100) {
    lcd.print("Left");
    delay(500);
    lcd.clear();
  }
  if (analogRead(0) > 450 & analogRead(0) < 550) {
    lcd.print("UP");
    delay(500);
    lcd.clear();
  }
   if (analogRead(0) > 200 & analogRead(0) < 300) {
    lcd.print("Down");
    delay(500);
    lcd.clear();
  }
   if (analogRead(0) > 1500 & analogRead(0) < 1700) {
    lcd.print("OK");
    delay(500);
    lcd.clear();
  }
}
