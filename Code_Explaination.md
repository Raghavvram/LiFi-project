### 1. **Including Libraries and Defining Pins**
   ```cpp
   #include <LiquidCrystal.h>

   LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
   #define LDR_PIN 8
   ```
   - **Explanation**: This block imports the `LiquidCrystal` library to control an LCD display. It creates an `lcd` object with the specified pins (2, 3, 4, 5, 6, and 7) for communication. The `LDR_PIN` is set to 8, where the LDR sensor is connected to read the light signal.

### 2. **Initializing Variables**
   ```cpp
   String binaryData = "";
   bool recording = false;
   int startCount = 0;
   int endCount = 0;
   ```
   - **Explanation**: Initializes variables to store the binary data, track whether recording is active, and count consecutive high or low signals (`startCount` and `endCount`) to determine when to start or stop recording.

### 3. **Setup Function**
   ```cpp
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
   ```
   - **Explanation**: Sets up serial communication at 9600 baud rate, configures the LDR pin as an input with a pull-up resistor, and initializes the LCD with a 16x2 display size. A message is displayed briefly, then the LCD shows “Waiting for signal” until data is received.

### 4. **Main Loop for Recording Data**
   ```cpp
   void loop() {
     int ldrValue = digitalRead(LDR_PIN);
   ```
   - **Explanation**: Reads the current signal from the LDR sensor, storing it in `ldrValue`. This value will determine if a binary `1` (light detected) or `0` (no light) is received.

### 5. **Starting the Recording**
   ```cpp
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
   }
   ```
   - **Explanation**: Checks if recording has not started (`recording` is `false`). If 12 consecutive binary `1`s (high signals) are detected, recording starts by clearing `binaryData` and updating the LCD to show “Recording…”.

### 6. **Recording the Data and Checking for Stop Condition**
   ```cpp
   else {
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
   ```
   - **Explanation**: If recording is active, the program appends each binary `ldrValue` (either `0` or `1`) to `binaryData`. When 12 consecutive `0`s are detected, recording stops. The recorded data (minus the last 12 `0`s) is then decoded into ASCII text, displayed on the LCD, and output via serial.

### 7. **Function to Convert Binary String to ASCII with Parity Check**
   ```cpp
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
   ```
   - **Explanation**: This function converts the binary string data into ASCII text. It processes 9 bits at a time: the first 8 bits are the ASCII value, and the 9th bit is a parity bit for error-checking. If the calculated parity matches the provided parity bit, the character is added to the decoded text. Otherwise, a “Transmission Error” message is shown on the LCD. 

### 8. **Delay for Processing**
   ```cpp
   delay(100); // Delay for processing
   ```
   - **Explanation**: This delay prevents the loop from running too quickly, allowing stable readings and processing time for each recorded binary signal.
