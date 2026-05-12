# ESP32 RFID Vault

[![Platform](https://img.shields.io/badge/platform-ESP32-blue)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B-orange)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()

## The short story

I built a simple RFID lock. You tap your authorised card, the servo unlocks for two seconds, the buzzer gives a happy beep, and the LCD shows “Access Granted”. Tap a wrong card, and it beeps sadly and says “Access Denied”.

If someone tries three wrong cards in a row, the system locks itself for 30 seconds – just to be annoying (or secure, depending on your point of view).

No keypad. No PIN. Just tap and (hopefully) open.

## Features that actually work

-  Reads your RFID card (you set the authorised UID in the code)
-  If card is authorised → servo rotates (unlocks), buzzer beeps happily, LCD welcoms you
-  If card is unauthorised → LCD kicks you, buzzer complains, servo stays put
-  After 3 unauthorised cards → lockout for 30 seconds (LCD shows a countdown)
-  The LCD guides you: “Scan Card” → “Access Granted” or “Access Denied” → “Locked” (if needed)

## What you need (hardware)

| Component | How many | Notes |
|-----------|----------|-------|
| ESP32 board | 1 | (we don't use WiFi here) |
| RFID‑RC522 module | 1 | comes with a card or tag |
| Servo motor (SG90 or similar) | 1 | to pretend it's a lock |
| Passive buzzer | 1 | makes beeps |
| 16x2 LCD (parallel) | 1 | the 16‑pin kind |
| 10k potentiometer | 1 | for LCD contrast |
| 220Ω resistor | 1 | for LCD backlight (optional) |
| Breadboard & jumper wires | many | you know the drill |

## Wiring (keep it simple)

I tried to pick pins that don't fight each other.

| Component | Pin | ESP32 GPIO |
|-----------|-----|------------|
| **LCD** | RS | 27 |
|         | EN | 14 |
|         | D4 | 26 |
|         | D5 | 25 |
|         | D6 | 33 |
|         | D7 | 32 |
|         | VSS | GND |
|         | VDD | 5V |
|         | V0 | potentiometer middle |
|         | RW | GND |
|         | LED+ | 5V (via 220Ω) |
|         | LED- | GND |
| **RFID** | SDA | 5 |
|          | SCK | 18 |
|          | MOSI | 23 |
|          | MISO | 19 |
|          | RST | 22 |
|          | 3.3V | 3.3V |
|          | GND | GND |
| **Servo** | Signal | 2 |
|           | VCC | 5V (external if big servo) |
|           | GND | GND |
| **Buzzer** | + | 3 |
|            | - | GND |

> **Servo power tip:** If the servo twitches or resets the ESP32, use a separate 5V supply (e.g., a phone charger). The ESP32’s 5V pin can handle a tiny servo (SG90) but not a heavy one.

## How to make it yours (set your authorised card)

### Find your card's UID

Upload this tiny sketch once:

```cpp
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Tap your card");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    rfid.PICC_HaltA();
  }
}
```

Open Serial Monitor (115200 baud), tap your card, and copy the UID (e.g., AB CD 12 34).

Change the code

In the main sketch (rfid_vault.ino), find this line and replace with your UID:

```cpp
String authorisedCard = "AB CD 12 34";   // your card's UID
```

How to use it

1. Upload the code to your ESP32.
2. The LCD shows “Scan Card”.
3. Hold your authorised RFID card near the reader.
4. If it matches → servo turns, buzzer chirps happily, LCD shows “Access Granted”. After 2 seconds, servo returns to locked position.
5. If it's a wrong card → LCD shows “Access Denied”, buzzer complains, servo does nothing.
6. After 3 wrong cards → system locks for 30 seconds (LCD shows a countdown). You cannot scan any card during that time.

That's it. No PIN, no keypad, just tap.

What the code does (plain English)

· The loop() constantly waits for a new RFID card.
· When a card is detected, it reads the UID.
· Compares it with the authorisedCard you set.
· If match → unlock servo, beep happy, show success.
· If not → increment a wrong‑card counter, beep sad, show denial.
· If counter reaches 3 → lockout timer starts (30 seconds). During lockout, the system ignores any card.

It's simple, but it works.

Troubleshooting (things I ran into)

Problem What to do
LCD shows nothing or blocks Turn the potentiometer slowly. Check 5V and GND.
RFID doesn't read Double‑check SPI pins (5,18,23,19,22). Keep card within 2‑3 cm.
Servo doesn't move Try a different GPIO (2 is fine). Check external power if it's a big servo.
Lockout triggers immediately You probably tapped three wrong cards – normal. Wait 30 seconds.
“AuthorisedCard” not working Copy the UID exactly as shown in the Serial Monitor (case and spaces matter).

Future ideas (if you get bored)

· Store multiple authorised cards in an array or EEPROM.
· Add a real‑time clock (RTC) to log access attempts.
· Replace servo with a solenoid or a real deadbolt.
· Send a Telegram message when someone tries a wrong card.

License

MIT – use it, break it, fix it, don't blame me.

Author

George Tomeh

---

P.S. The servo doesn’t actually lock your door. It's just a demonstration. Please don't use this for real security – your cat could still push it open.
