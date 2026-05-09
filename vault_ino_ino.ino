/*
 RFID vault System – Step 5 (FINAL)
 Full system: valid card -> unlock + happy beep
              invalid card -> sad beep + counter
              3 invalids -> lockout 30s
 */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <ESP32Servo.h>

LiquidCrystal lcd(27, 14, 26, 25, 33, 32);

#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

String correctCard = "AA AA AA AA";   // change me

Servo lockServo;

const int servoPin = 2;

const int buzzerPin = 12;

int wrongCount = 0;

unsigned long lockoutUntil = 0;

const unsigned long LOCKOUT_TIME = 30000;   // 30 seconds

void happy() {
  digitalWrite(buzzerPin, HIGH);
  delay(200);
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

void lockoutWarning() {
  for (int i = 0; i < 3; i++) { 
    digitalWrite(buzzerPin, HIGH);
    delay(50); 
    digitalWrite(buzzerPin, LOW);
    delay(50); 
  }
}

void setup() {
  Serial.begin(115200);
  
  SPI.begin();
   
  rfid.PCD_Init();
   
  lcd.begin(16, 2);
    
  lockServo.attach(servoPin);
    
  pinMode(buzzerPin, OUTPUT);
   
  lockServo.write(0);

  lcd.print(" Vault System ");
  lcd.setCursor(0, 1);
  lcd.print(" Tap card");
  delay(2000);
  lcd.clear();
  lcd.print(" Ready");
}

void loop() {
  // lockout handler
  if (wrongCount >= 3 && millis() < lockoutUntil) {
    lcd.clear();
    lcd.print(" vault locked");
    lcd.setCursor(0, 1);
    lcd.print(" Wait ");
    lcd.print((lockoutUntil - millis()) / 1000);
    lcd.print("s");
    delay(500);
    return;
  }
  else if (wrongCount >= 3 && millis() >= lockoutUntil) {
    wrongCount = 0;
    lcd.clear();
    lcd.print(" Ready");
    lcd.setCursor(0, 1);
    lcd.print(" Tap card");
  }

  // RFID scan handler
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
    // valid card : reset wrong counter and unlock
    wrongCount = 0;
    lcd.print(" vault opened ");
    lcd.setCursor(0, 1);
    lcd.print("      Hi      ");
    happy();
    lockServo.write(90);      // unlock
    delay(2000);
    lockServo.write(0);       // lock again
  } else {
    // invalid card : increment counter
    wrongCount++;
    lcd.print(" you liar   ");
    lcd.setCursor(0, 1);
    lcd.print(" wrong card");
    sad();
    delay(1500);

    if (wrongCount >= 3) {
      lcd.clear();
      lcd.print(" vault locked");
      lcd.setCursor(0, 1);
      lcd.print(" too many lies");
      lockoutWarning();
      lockoutUntil = millis() + LOCKOUT_TIME;
      delay(2000);
      lcd.clear();
      lcd.print(" locked");
    }
  }

  lcd.clear();
  lcd.print(" Ready");
  lcd.setCursor(0, 1);
  lcd.print(" Tap card");
  rfid.PICC_HaltA();
}