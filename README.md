# Arduino
# Smart Car Parking System with 2FA 🚗🔒

An automated, embedded security system designed to manage parking access and monitor slot availability in real-time. Built on an Arduino Mega, this project enforces **Two-Factor Authentication (2FA)** for vehicle entry by combining physical token verification (RFID) with a secondary digital passcode (IR Remote), while dynamically managing parking capacity.

## 📑 Project Overview
Traditional boom barrier systems rely on a single point of failure (often just an RFID card). This project scales down a commercial-grade security system to an Arduino level, requiring a driver to verify both "something they have" (an RFID tag) and "something they know" (a 4-digit PIN) before the barrier opens. 

**Key Features:**
* **Dynamic Capacity Management:** Real-time tracking of available parking spots (Max 4 slots). If the lot is full, the system automatically rejects entry attempts and notifies the user via the LCD.
* **Two-Factor Authentication:** Seamless integration of an MFRC522 RFID reader and an IR Receiver.
* **Automated Barrier Logic:** Synchronized servo motor control triggered sequentially by entrance and exit IR sensors to prevent tailgating.
* **Timeout & Security Protocols:** Implemented timeout logic if a user fails to input the password in time, ensuring the system resets safely.

## 🛠️ Hardware Architecture
The system logic is processed by an **Arduino Mega 2560**, wired to the following components:
* **MFRC522 RFID Module** (SPI interface) for primary authentication.
* **IR Receiver & Remote** for secondary PIN entry.
* **3x IR Obstacle Avoidance Sensors** (IR1 for entry detection, IR2 for exit detection, IR3 for remote signal reception).
* **Servo Motor** for boom barrier simulation.
* **16x2 I2C LCD Display** for user interface and capacity monitoring.

## 💻 Software & Libraries
The codebase is written in C++ for the Arduino environment. Ensure the following libraries are installed via the Arduino Library Manager before compiling:
* `Wire.h` (I2C communication)
* `LiquidCrystal_I2C.h` (LCD display)
* `Servo.h` (Barrier control)
* `SPI.h` (Serial Peripheral Interface for RFID)
* `MFRC522.h` (RFID reading)
* `IRremote.h` (Infrared signal decoding)

## 🔄 System Workflow
### Entry Sequence:
1. **Detection:** A vehicle approaches and triggers the first IR sensor (`IR1`).
2. **Capacity Check:** The system checks available slots. If full (0 slots), the LCD displays a "Parking Full" message and the gate remains locked. If at least 1 slot is open, it proceeds to authentication.
3. **First Factor (RFID):** The LCD prompts the user to scan their RFID tag. The system checks the UID against authorized users.
4. **Second Factor (PIN):** If the RFID is valid, the LCD prompts for a PIN via the IR Remote. The user inputs the code (default `1234`), with inputs masked by asterisks `*` on the screen.
5. **Access Granted:** The servo smoothly opens the barrier. 
6. **Clearance:** The system waits for the vehicle to pass the second sensor (`IR2`), then smoothly closes the barrier and decreases the available slot count by 1.

### Exit Sequence:
1. **Detection:** A departing vehicle triggers the inside sensor (`IR2`).
2. **Access Granted:** The barrier opens automatically for exit.
3. **Clearance:** Once the vehicle passes the outside sensor (`IR1`), the barrier closes, and the available slot count increases by 1.

## 🚀 How to Run the Project
1. Clone this repository to your local machine.
2. Open `project.ino` in the Arduino IDE.
3. Connect the Arduino Mega and verify the pin mappings in the code:
   * `SS_PIN` -> 10, `RST_PIN` -> 9
   * `SERVO_PIN` -> 2, `IR_PIN` -> 8
4. Compile and upload the code to the Arduino Mega.
5. Open the Serial Monitor (9600 baud) for debugging if necessary.
