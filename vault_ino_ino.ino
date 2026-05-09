/*
 RFID Access Control – Step 4
 Now with physical action: servo moves when access granted,
 buzzer gives different tones for success/failure.
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>

LiquidCrystal lcd(27, 14, 26, 25, 33, 32);

#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

String correctCard = "AB CD 12 34";   // change me

Servo lockServo;

const int servoPin = 2;

const int buzzerPin = 12;

void happy() {
  digitalWrite(buzzerPin, HIGH);
  delay(400);
  digitalWrite(buzzerPin, LOW);
}

void sad() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH);
  delay(200);
  digitalWrite(buzzerPin, LOW);
}

void setup() {
  Serial.begin(115200);
  
  SPI.begin();
  
  rfid.PCD_Init();
  
  lcd.begin(16, 2);
  
  lockServo.attach(servoPin);
  
  pinMode(buzzerPin, OUTPUT);
  
  lockServo.write(0);

  lcd.print("Access Control");
  lcd.setCursor(0, 1);
  lcd.print("Tap card");
  delay(2000);
  lcd.clear();
  lcd.print("Ready");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String cardID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    cardID += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) cardID += " ";
  }
  cardID.toUpperCase();

  lcd.clear();
  if (cardID == correctCard) {
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("vault opened");
    happy();
    lockServo.write(90);    // unlock
    delay(2000);
    lockServo.write(0);     // lock again
  } else {
    lcd.print("you liar");
    lcd.setCursor(0, 1);
    lcd.print("wrong card");
    sad();
    delay(1500);
  }
  lcd.clear();
  lcd.print("Ready");

  rfid.PICC_HaltA();
}