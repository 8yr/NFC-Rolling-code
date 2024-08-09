#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <avr/wdt.h>

#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;

const byte BLOCK_ADDR = 1;
MFRC522::MIFARE_Key key;

const byte allowedUIDs[][4] = {
  {0xDE, 0xAD, 0xBE, 0xEF},
  {0xCA, 0xFE, 0xBA, 0xBE},
  {0x03, 0x7E, 0x7F, 0x2F}
};
const int allowedUIDCount = sizeof(allowedUIDs) / sizeof(allowedUIDs[0]);

byte storedCodes[allowedUIDCount][16];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  myservo.attach(3);
  myservo.write(0);

  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  memset(storedCodes, 0, sizeof(storedCodes));
}

bool isAllowedUID(byte* uid, byte uidSize, int& index) {
  for (int i = 0; i < allowedUIDCount; i++) {
    if (memcmp(uid, allowedUIDs[i], uidSize) == 0) {
      index = i;
      return true;
    }
  }
  return false;
}

void printUID(byte* uid, byte uidSize) {
  for (byte i = 0; i < uidSize; i++) {
    Serial.print(uid[i] < 0x10 ? " 0" : " ");
    Serial.print(uid[i], HEX);
  }
  Serial.println();
}

void generateRandomCode(byte* code, int length) {
  for (int i = 0; i < length; i++) {
    code[i] = random(0, 256);
  }
}

void processCard(int uidIndex) {
  byte buffer[18];
  byte size = sizeof(buffer);

  if (mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BLOCK_ADDR, &key, &(mfrc522.uid)) != MFRC522::STATUS_OK) {
    Serial.println(F("Authentication failed"));
    Serial.println();
    Serial.println(F("*****"));
    Serial.println();
    return;
  }

  if (mfrc522.MIFARE_Read(BLOCK_ADDR, buffer, &size) != MFRC522::STATUS_OK) {
    Serial.println(F("Read failed: A MIFARE PICC responded with NAK"));
    Serial.println();
    Serial.println(F("*****"));
    Serial.println();
    delay(3000);
    return;
  }

  bool firstTime = true;
  for (int i = 0; i < 16 && firstTime; i++) {
    if (storedCodes[uidIndex][i] != 0) firstTime = false;
  }

  if (firstTime) {
    Serial.println(F("First time use. Storing random code."));
    generateRandomCode(storedCodes[uidIndex], 16);
    if (mfrc522.MIFARE_Write(BLOCK_ADDR, storedCodes[uidIndex], 16) != MFRC522::STATUS_OK) {
      Serial.println(F("Write failed"));
      Serial.println();
      Serial.println(F("*****"));
      Serial.println();
    } else {
      Serial.println(F("New code written to card."));
      tone(8, 1200, 200);
      delay(220);
      tone(8, 1200, 200);
      delay(220);
      tone(8, 1200, 200);
      Serial.println();
      Serial.println(F("*****"));
      Serial.println();
    }
  } else if (memcmp(buffer, storedCodes[uidIndex], 16) == 0) {
    Serial.println(F("Code is correct!"));
    tone(8, 1200, 200);
    delay(220);
    tone(8, 1200, 200);
    generateRandomCode(storedCodes[uidIndex], 16);
    if (mfrc522.MIFARE_Write(BLOCK_ADDR, storedCodes[uidIndex], 16) != MFRC522::STATUS_OK) {
      Serial.println(F("Write failed"));
      Serial.println();
      Serial.println(F("*****"));
      Serial.println();
    } else {
      Serial.println(F("New code written to card."));
      Serial.println();
      Serial.println(F("*****"));
      Serial.println();
    }
  } else {
    Serial.println(F("Code is not correct!"));
    Serial.println();
    Serial.println(F("*****"));
    Serial.println();
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(3000);
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command == "R") {
      Serial.println(F("Resetting..."));
        for (int i = 0; i < 100; i++) {
          Serial.println();
        }
      wdt_enable(WDTO_15MS);
      while (1);
    }
  }

  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("Card UID:");
  printUID(mfrc522.uid.uidByte, mfrc522.uid.size);

  int uidIndex;
  if (isAllowedUID(mfrc522.uid.uidByte, mfrc522.uid.size, uidIndex)) {
    processCard(uidIndex);
  } else {
    Serial.println(F("UID not allowed."));
    Serial.println();
    Serial.println(F("*****"));
    Serial.println();
    tone(8, 1300, 800);
    delay(3000);
  }
}
