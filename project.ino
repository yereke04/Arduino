#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <IRremote.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 2
#define IR_PIN 8 

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myservo;

const int IR_RECEIVE_PIN = IR_PIN;
String correctPassword = "1234";

int IR1 = 4;
int IR2 = 3;
int Slot = 4;
const int MaxSlots = 4;
bool carEntering = false;
bool carExiting = false;

String authorizedUID = "23 92 6B 9D";

// function declarations
bool askIRPassword();
void smoothOpenGate();
void smoothCloseGate();
void waitForSensor(int pin);
int rfidAndPasswordCheck();

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  myservo.attach(SERVO_PIN);
  myservo.write(100);  // gate initially closed

  SPI.begin();
  rfid.PCD_Init();
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  lcd.setCursor(0, 0);
  lcd.print("  SMART ");
  lcd.setCursor(0, 1);
  lcd.print(" PARKING SYSTEM ");
  delay(3000);
  lcd.clear();
}

void loop() {
  if (digitalRead(IR1) == LOW && !carEntering && !carExiting) {
    carEntering = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Car Detected!");
    delay(1000);

    if (Slot > 0) {
      int pass = rfidAndPasswordCheck();
      if (pass == 1) {
        smoothOpenGate();
        Slot--;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Gate Opened!");
        delay(1500);
        waitForSensor(IR2);
        smoothCloseGate();
        lcd.clear();
        lcd.print("Gate Closed!");
        delay(1500);
      }
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  Parking Full ");
      delay(2500);
    }

    carEntering = false;
  }

  if (digitalRead(IR2) == LOW && !carExiting && !carEntering) {
    if (Slot == MaxSlots) {
      lcd.clear();
      lcd.print(" No Cars Left! ");
      delay(2500);
    } else {
      carExiting = true;
      smoothOpenGate();
      Slot++;
      lcd.clear();
      lcd.print("Car Exiting...");
      delay(1500);
      waitForSensor(IR1);
      smoothCloseGate();
      lcd.clear();
      lcd.print("Gate Closed!");
      delay(1500);
      carExiting = false;
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("   WELCOME!     ");
  lcd.setCursor(0, 1);
  lcd.print("Slots Left: ");
  lcd.print(Slot);
  delay(1000);
}

int rfidAndPasswordCheck() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Please,");
  lcd.setCursor(1, 1);
  lcd.print("Put your card");

  while (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(350);
  }

  String scannedUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) scannedUID += "0"; // leading zero
    scannedUID += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) scannedUID += " ";
  }
  scannedUID.toUpperCase();

  lcd.clear();
  if (scannedUID == authorizedUID) {
    lcd.print("Card valid");
    delay(1500);
    if (askIRPassword()) {
      return 1;
    } else {
      lcd.clear();
      lcd.print("Access Denied");
      delay(1500);
      return 0;
    }
  } else {
    lcd.print("Card invalid!");
    delay(1500);
    return 0;
  }
}

bool askIRPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");

  String input = "";
  unsigned long lastInputTime = millis();
  const unsigned long timeout = 15000;

  while (millis() - lastInputTime < timeout) {
    if (IrReceiver.decode()) {
      uint8_t command = IrReceiver.decodedIRData.command;
      Serial.print("Command: 0x");
      Serial.println(command, HEX);

      switch (command) {
        case 0xC: input += '1'; break;
        case 0x18: input += '2'; break;
        case 0x5E: input += '3'; break;
        case 0x8: input += '4'; break;
        case 0x1C: input += '5'; break;
        case 0x5A: input += '6'; break;
        case 0x42: input += '7'; break;
        case 0x52: input += '8'; break;
        case 0x4A: input += '9'; break;
        case 0x16: input += '0'; break;
        case 0x19: input = ""; break; // red button for clearing
        case 0x46: // mode button for verifying
          if (input == correctPassword) {
            lcd.clear();
            lcd.print("Access Granted");
            delay(1500);
            IrReceiver.resume(); // resume after success
            return true;
          } else {
            lcd.clear();
            lcd.print("Wrong Password");
            delay(1500);
            IrReceiver.resume(); // resume after failure
            return false;
          }
      }

      // show asterisks
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      for (unsigned int i = 0; i < input.length(); i++) lcd.print("*");

      lastInputTime = millis();
      IrReceiver.resume();
    }
  }

  lcd.clear();
  lcd.print("Timeout!");
  delay(1500);
  IrReceiver.resume();
  return false;
}

void smoothCloseGate() {
  for (int pos = 0; pos <= 100; pos++) {
    myservo.write(pos);
    delay(10);
  }
}

void waitForSensor(int pin) {
  while (digitalRead(pin) == HIGH) delay(100);
  while (digitalRead(pin) == LOW) delay(100);
}

void smoothOpenGate() {
  for (int pos = 100; pos >= 0; pos--) {
    myservo.write(pos);
    delay(10);
  }
}
