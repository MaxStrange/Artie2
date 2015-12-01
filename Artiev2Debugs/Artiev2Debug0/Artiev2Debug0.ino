#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

const char DRIVE_A = 'q';
const char BACK_A = 'a';
const char DRIVE_B = 'w';
const char BACK_B = 's';
const char DRIVE_C = 'e';
const char BACK_C = 'd';
const char DRIVE_D = 'r';
const char BACK_D = 'f';
const char OPEN_CLAW = 't';
const char CLOSE_CLAW = 'g';
const char STOP = 'y';
const char PRINT_POT = 'h';

const char END_OF_ACTION = 'P';
const char SEND_ID = 'G';
const char TEST_CONNECTION = 't';

Adafruit_MotorShield AFMS_A = Adafruit_MotorShield();
Adafruit_MotorShield AFMS_B = Adafruit_MotorShield(0x61);

Adafruit_DCMotor* motorA = AFMS_A.getMotor(1);
Adafruit_DCMotor* motorB = AFMS_A.getMotor(2);
Adafruit_DCMotor* motorC = AFMS_A.getMotor(3);
Adafruit_DCMotor* motorD = AFMS_A.getMotor(4);
Adafruit_DCMotor* motorE = AFMS_B.getMotor(1);

Adafruit_DCMotor* motorArray[] = { motorA, motorB, motorC, motorD, motorE };

struct joint
{
  Adafruit_DCMotor* motor;
  unsigned int potPin;
  unsigned int minimum;
  unsigned int maximum;
};

struct joint joint_A = { motorA, 4, 580, 630 };
struct joint joint_B = { motorB, 3, 237, 911 };
struct joint joint_C = { motorC, 2, 375, 900 };
struct joint joint_D = { motorD, 1, 157, 663 };
struct joint claw_ = { motorE, 0, 319, 430 };

struct joint* jointA = &joint_A;
struct joint* jointB = &joint_B;
struct joint* jointC = &joint_C;
struct joint* jointD = &joint_D;
struct joint* claw = &claw_;

struct joint* jointArray[] = { jointA, jointB, jointC, jointD, claw };

boolean loopFlag = true;

void setup()
{
  Serial.begin(9600);
  AFMS_A.begin();
  AFMS_B.begin();
}

void loop()
{
  while (Serial.available())
  {
    readSerial();
  }
  if (loopFlag)
  {
    Serial.write(END_OF_ACTION);
    loopFlag = false;
  }
}

void readSerial()
{
  unsigned int inChar = Serial.read();
  switch (inChar)
  {
    case DRIVE_A:
            driveJoint(jointA, FORWARD);
            break;
    case BACK_A:
            driveJoint(jointA, BACKWARD);
            break;
    case DRIVE_B:
            driveJoint(jointB, FORWARD);
            break;
    case BACK_B:
            driveJoint(jointB, BACKWARD);
            break;
    case DRIVE_C:
            driveJoint(jointC, FORWARD);
            break;
    case BACK_C:
            driveJoint(jointC, BACKWARD);
            break;
    case DRIVE_D:
            driveJoint(jointD, FORWARD);
            break;
    case BACK_D:
            driveJoint(jointD, BACKWARD);
            break;
    case OPEN_CLAW:
            driveJoint(claw, BACKWARD);
            break;
    case CLOSE_CLAW:
            driveJoint(claw, FORWARD);
            break;
    case PRINT_POT:
            printAllPots();
            break;
    case STOP:
            stopAllMotors();
            break;
    default:
            stopAllMotors();
            break;
  }
}

void driveJoint(struct joint* jointToDrive, uint8_t dir)
{
  readAndPrint(jointToDrive->potPin);
  runMotor(jointToDrive->motor, dir);
  loopFlag = true;
}

void printAllPots()
{
  for (int i = 0; i < 5; i++)
  {
  readAndPrint(jointArray[i]->potPin);
  }
}

void readAndPrint(unsigned int pot)
{
  unsigned int potValue = analogRead(pot);
  Serial.print("\nPot ");
  Serial.print(pot);
  Serial.print(": ");
  Serial.println(potValue);
}

void runMotor(Adafruit_DCMotor* motor, uint8_t dir)
{
  motor->setSpeed(100);
  motor->run(dir);
  delay(100);
}

void stopAllMotors()
{
  for (int i = 0; i < 5; i++)
  {
    jointArray[i]->motor->setSpeed(0);
  }
}
