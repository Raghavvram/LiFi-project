#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define LDR_PIN 8

String binaryData = "";
bool recording = false;
int startCount = 0;
int endCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LDR_PIN, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("LiFi Project");
  delay(1000);
  lcd.clear();
  lcd.print("Waiting for signal");
}

void loop() {
  int ldrValue = digitalRead(LDR_PIN);

  if (!recording) {
    if (ldrValue == 1) {
      startCount++;
    } else {
      startCount = 0;
    }

    if (startCount >= 12) { // Start recording after 12 consecutive 1s
      recording = true;
      binaryData = ""; // Clear previous data
      Serial.println("Recording started");
      lcd.clear();
      lcd.print("Recording...");
      startCount = 0; // Reset start count
    }
  } else {
    binaryData += String(ldrValue);

    if (ldrValue == 0) {
      endCount++;
    } else {
      endCount = 0;
    }

    if (endCount >= 12) { // Stop recording after 12 consecutive 0s
      recording = false;
      Serial.println("Recording ended");
      String recordedData = binaryData.substring(0, binaryData.length() - 12); // Exclude the last 12 zeros
      Serial.print("Recorded binary data: ");
      Serial.println(recordedData);
      
      // Convert binary string to normal string using ASCII and 9-bit parity check
      String normalString = binaryToString(recordedData);
      Serial.print("Decoded text: ");
      Serial.println(normalString);

      lcd.clear();
      lcd.print(normalString); // Display decoded text on LCD until next string is received
      endCount = 0; // Reset end count
    }
  }

  delay(100); // Delay for processing
}

// Function to convert binary string to normal string using ASCII and parity check
String binaryToString(String binary) {
  String text = "";
  for (int i = 0; i < binary.length(); i += 9) {
    if (i + 9 <= binary.length()) {
      String byteString = binary.substring(i, i + 8);
      char parityBit = binary.charAt(i + 8);
      char character = (char) strtol(byteString.c_str(), NULL, 2);
      
      // Calculate parity
      int parity = 0;
      for (int j = 0; j < 8; j++) {
        parity ^= (byteString.charAt(j) - '0');
      }

      // Check if parity matches
      if (parity == (parityBit - '0')) {
        text += character; // Append character if parity matches
      } else {
        Serial.println("Parity error detected!");
        lcd.print("Transmission Error, Please Retransmit");
      }
    }
  }
  return text;
}
