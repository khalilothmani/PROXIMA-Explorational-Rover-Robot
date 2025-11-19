#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

// RF24 CE, CSN pins
RF24 radio(7, 8);
const byte address[6] = "00001";

// Motor driver pins (L293D)
int enA = 9;
int in1 = 6;
int in2 = 5;

int enB = 10;
int in3 = 4;
int in4 = 3;

// Camera servos
Servo servoX; // Pan
Servo servoY; // Tilt

// Incoming data structure
struct DataPacket {
  int moveX;
  int moveY;
  int headX;
  int headY;
};

DataPacket packet;

void setup() {
  // Motor pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Servos
  servoX.attach(2);
  servoY.attach(13);

  // Radio setup
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening(); // Rover = receiver
}

void driveMotors(int speed, int turn) {
  int leftMotor = speed + turn;
  int rightMotor = speed - turn;

  leftMotor = constrain(leftMotor, -255, 255);
  rightMotor = constrain(rightMotor, -255, 255);

  // LEFT MOTOR
  if (leftMotor > 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  analogWrite(enA, abs(leftMotor));

  // RIGHT MOTOR
  if (rightMotor > 0) {
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
  analogWrite(enB, abs(rightMotor));
}

void loop() {
  if (radio.available()) {
    radio.read(&packet, sizeof(packet));

    // Convert 0–1023 to -255 to +255
    int speed = map(packet.moveY, 0, 1023, -255, 255);
    int turn  = map(packet.moveX, 0, 1023, -255, 255);

    driveMotors(speed, turn);

    // Camera head movement
    int camX = map(packet.headX, 0, 1023, 0, 180);
    int camY = map(packet.headY, 0, 1023, 0, 180);

    servoX.write(camX);
    servoY.write(camY);
  }
}
