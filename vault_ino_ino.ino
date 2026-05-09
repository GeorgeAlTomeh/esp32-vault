/*
 RFID vault – Step 1
 Just a demo: servo, buzzer, LCD
 */

#include <LiquidCrystal.h>
#include <ESP32Servo.h>

LiquidCrystal lcd(27, 14, 26, 25, 33, 32);

Servo lockServo;

const int servoPin = 2;
const int buzzerPin = 12;

void setup() {
  lcd.begin(16, 2);
    
  lockServo.attach(servoPin);
  
  pinMode(buzzerPin, OUTPUT);

  lcd.print("Vault System");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);

  // test servo
  lockServo.write(90);
  delay(1000);
  lockServo.write(0);

  // test buzzer
  digitalWrite(buzzerPin, HIGH);
  delay(300);
  digitalWrite(buzzerPin, LOW);

  lcd.clear();
  lcd.print(" Ready");
  lcd.setCursor(0, 1);
  lcd.print("Tap card)");
}

void loop() {
  // empty
}