#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 49

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  SPI.begin();  // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522
  Serial.println(F("Place your card/fob on the reader to read the data."));
}

void loop() {
  // Check for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  MFRC522::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  String text = "";

  // Read from blocks 4 to 6
  for (byte block = 4; block <= 6; block++) {
    byte buffer[18];
    byte size = sizeof(buffer);
    memset(buffer, 0, size);  // Clear the buffer

    if (readFromCard(block, buffer, key, size)) {
      for (byte i = 0; i < 16; i++) {  // Convert to string
        if (buffer[i] != 0) {
          text += (char)buffer[i];
        }
      }
    } else {
      Serial.print(F("Failed to read from block ")); Serial.println(block);
    }
  }

  // Output the data
  Serial.println(text);

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

  delay(1000); // Delay to prevent continuous reading
}

bool readFromCard(byte block, byte *buffer, MFRC522::MIFARE_Key key, byte &size) {
  // Authenticate
  mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &mfrc522.uid);
  // Read
  MFRC522::StatusCode status = mfrc522.MIFARE_Read(block, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    return false;
  }
  return true;
}
