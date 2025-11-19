#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <LiquidCrystal.h>

// RF24 radio CE, CSN
RF24 radio(7, 8);
const byte address[6] = "00001";

// LCD pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Joystick pins
int joyMoveX = A0; // Movement X
int joyMoveY = A1; // Movement Y
int joyHeadX = A2; // Camera X
int joyHeadY = A3; // Camera Y

// Data packet to send
struct DataPacket {
  int moveX;
  int moveY;
  int headX;
  int headY;
};

DataPacket packet;

void setup() {
  lcd.begin(16, 2);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); // Remote = transmitter

  lcd.print("PROXIMA Remote");
  delay(1000);
}

void loop() {
  // Read joystick analog values
  packet.moveX = analogRead(joyMoveX);
  packet.moveY = analogRead(joyMoveY);
  packet.headX = analogRead(joyHeadX);
  packet.headY = analogRead(joyHeadY);

  // Send packet to rover
  radio.write(&packet, sizeof(packet));

  // Display values on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MX:");
  lcd.print(packet.moveX);
  lcd.print(" MY:");
  lcd.print(packet.moveY);
  lcd.setCursor(0, 1);
  lcd.print("HX:");
  lcd.print(packet.headX);
  lcd.print(" HY:");
  lcd.print(packet.headY);

  delay(100);
}
