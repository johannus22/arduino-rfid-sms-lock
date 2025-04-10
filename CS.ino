#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>  //  RFID library

#define relay 4
#define red 16
#define green 15
#define RST_PIN 9      // Define RFID reset pin
#define SS_PIN 10      // Define RFID SDA pin

const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 13, 12, 11, 10 };
byte colPins[COLS] = { 9, 8, 7, 6 };
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial sim800l(3, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // MFRC522 RFID instance

int irsensor = A0;
int otp;
String otpstring = "";
String authorizedUID = "A1B2C3D4";  // UID

void setup() {
  pinMode(irsensor, INPUT_PULLUP);
  sim800l.begin(4800);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Serial.print("Welcome to SIM800L Project");
  sim800l.println("AT");
  updateSerial();
  pinMode(relay, OUTPUT);
  pinMode(red, INPUT);
  pinMode(green, INPUT);
  digitalWrite(relay, LOW);
  delay(500);
  sim800l.println("AT+CSQ");
  updateSerial();
  delay(1000);

  SPI.begin();
  mfrc522.PCD_Init();  // Initialize RFID scanner
}

void updateSerial() {
  delay(500);
  while (Serial.available()) {
    sim800l.write(Serial.read());
  }
  while (sim800l.available()) {
    Serial.write(sim800l.read());
  }
}


//loop main method
void loop() {
  lcd.setCursor(0, 0);
  lcd.print("   OTP Based");
  lcd.setCursor(0, 1);
  lcd.print("   Door Lock");

  // Check for RFID Card
  if (scanRFID()) {
    unlockDoor();
  }

  // If IR sensor detects movement, send OTP
  if (digitalRead(irsensor) == LOW) {
    otp = random(2000, 9999);
    otpstring = String(otp);
    Serial.println(otpstring);
    while (digitalRead(irsensor) == LOW) {}

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" OTP Sent to");
    lcd.setCursor(0, 1);
    lcd.print(" Your Mobile");

    Serial.print("OTP is ");
    delay(100);
    Serial.println(otpstring);
    delay(100);
    SendSMS();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter OTP :");
    getotp();
  }
}

// Function to scan RFID card
bool scanRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;  // No card detected
  }

  String cardUID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    cardUID += String(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("Card UID: ");
  Serial.println(cardUID);

  if (cardUID.equalsIgnoreCase(authorizedUID)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Granted");
    lcd.setCursor(0, 1);
    lcd.print("Door Opening");
    return true;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
    delay(3000);
    return false;
  }
}

// Function to unlock the door
void unlockDoor() {
  digitalWrite(relay, HIGH);
  digitalWrite(red, HIGH);
  digitalWrite(green, LOW);
  delay(5000);
  digitalWrite(relay, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
}

void getotp() {
  String y = "";
  int a = y.length();
  while (a < 4) {
    char customKey = customKeypad.getKey();
    if (customKey) {
      lcd.setCursor(0, 1);
      y = y + customKey;
      lcd.print(y);
      a = y.length();
    }
  }
  Serial.print("Entered OTP is ");
  Serial.println(y);

  if (otpstring == y) {
    unlockDoor();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Access Failed");
    lcd.setCursor(0, 1);
    lcd.print("Try Again !!!");
    delay(3000);
  }
}

void SendSMS() {
  Serial.println("Sending SMS...");
  sim800l.print("AT+CMGF=1\r");
  delay(100);
  sim800l.print("AT+CSMP=17,167,0,0\r");
  delay(500);
  sim800l.print("AT+CMGS=\"+91xxxxxxxxxxx\"\r");
  delay(500);
  sim800l.print("Your OTP is " + otpstring + " Just Type OTP And Unlock The Door");
  delay(500);
  sim800l.print((char)26);
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);
}
