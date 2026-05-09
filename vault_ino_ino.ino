/*
 Step 3
 Checks if card UID is correct
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>

LiquidCrystal lcd(27, 14, 26, 25, 33, 32);

#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

// REPLACE WITH YOUR CARD'S UID
String correctCard = "AB CD 12 34";

Servo lockServo;

const int servoPin = 2;

const int buzzerPin = 3;

void setup() {
  Serial.begin(115200);
  
  SPI.begin();
  
  rfid.PCD_Init();
    
  lcd.begin(16, 2);
    
  lockServo.attach(servoPin);
  
  pinMode(buzzerPin, OUTPUT);
   
  lockServo.write(0);

  lcd.print("vault");
  lcd.setCursor(0, 1);
  lcd.print("Tap a card");
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
    lcd.print("vault opened");
    lcd.setCursor(0, 1);
    lcd.print("Welcome!");
  } else {
    lcd.print("you liar");
    lcd.setCursor(0, 1);
    lcd.print("wrong card");
  }
  delay(2000);
  lcd.clear();
  lcd.print("Ready");

  rfid.PICC_HaltA();
}