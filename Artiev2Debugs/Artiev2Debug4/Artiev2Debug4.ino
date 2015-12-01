#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

const char DRIVE_A = 'Q';
const char BACK_A = 'R';
const char DRIVE_B = 'S';
const char BACK_B = 'T';
const char DRIVE_C = 'U';
const char BACK_C = 'V';
const char DRIVE_D = 'W';
const char BACK_D = 'Y';
const char OPEN_CLAW = 'Z';
const char CLOSE_CLAW = 'a';
const char STOP = 'I';
const char PRINT_POT = 'K';
const char GET_CALIBRATION_DATA = 'J';
const char END_OF_ACTION = 'P';
const char SEND_ID = 'G';
const char TEST_CONNECTION = 't';
const char REMOVE_SAFETY = 'H';
const char FORMAT_FLAG_NUMBER = 'M';
const char SET_SAFETY = 'N';

const char PRINT_POT_A = 'b';
const char PRINT_POT_B = 'c';
const char PRINT_POT_C = 'd';
const char PRINT_POT_D = 'e';
const char PRINT_POT_CLAW = 'f';

const unsigned int PASS_LENGTH = 3;//Number of times format_flag_number is sent

const unsigned int DUTY_CYCLE = 100;
const unsigned int ACTION_LENGTH = 2000;

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

struct joint joint_A = { motorA, A0, 580, 630 };
struct joint joint_B = { motorB, A2, 237, 911 };
struct joint joint_C = { motorC, A3, 375, 900 };
struct joint joint_D = { motorD, A4, 157, 663 };
struct joint claw_ = { motorE, A5, 319, 430 };

struct joint* jointA = &joint_A;
struct joint* jointB = &joint_B;
struct joint* jointC = &joint_C;
struct joint* jointD = &joint_D;
struct joint* claw = &claw_;

struct joint* jointArray[] = { jointA, jointB, jointC, jointD, claw };

boolean calibrating = false;
boolean loopFlag = true;

void setup()
{
  Serial.begin(9600);
  AFMS_A.begin();
  AFMS_B.begin();
  
  fdevopen(&my_putc, 0);
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



int my_putc( char c, FILE *t) {
  Serial.write( c );
}



void readSerial()
{
  unsigned int inChar = Serial.read();
  switch (inChar)
  {
    case DRIVE_A:
            driveJoint(jointA, FORWARD, DRIVE_A);
            break;
    case BACK_A:
            driveJoint(jointA, BACKWARD, BACK_A);
            break;
    case DRIVE_B:
            driveJoint(jointB, FORWARD, DRIVE_B);
            break;
    case BACK_B:
            driveJoint(jointB, BACKWARD, BACK_B);
            break;
    case DRIVE_C:
            driveJoint(jointC, FORWARD, DRIVE_C);
            break;
    case BACK_C:
            driveJoint(jointC, BACKWARD, BACK_C);
            break;
    case DRIVE_D:
            driveJoint(jointD, FORWARD, DRIVE_D);
            break;
    case BACK_D:
            driveJoint(jointD, BACKWARD, BACK_D);
            break;
    case OPEN_CLAW:
            driveJoint(claw, BACKWARD, OPEN_CLAW);
            break;
    case CLOSE_CLAW:
            driveJoint(claw, FORWARD, CLOSE_CLAW);
            break;
    case REMOVE_SAFETY:
            removeSafetyForCalibration();
            break;
    case SET_SAFETY:
            setSafety();
            break;
    case GET_CALIBRATION_DATA:
            calibrate();
            break;
    case PRINT_POT:
            printAllPots();
            break;
    case TEST_CONNECTION:
            testConnection();
            break;
    case SEND_ID:
            sendID();
            break;
    case STOP:
            stopAllMotors();
            break;
    case PRINT_POT_A:
            printPotA();
            break;
    case PRINT_POT_B:
            printPotB();
            break;
    case PRINT_POT_C:
            printPotC();
            break;
    case PRINT_POT_D:
            printPotD();
            break;
    case PRINT_POT_CLAW:
            printPotClaw();
            break;
    default:
            stopAllMotors();
            break;
  }
}

void calibrate()
{
  char get_calibration_data_arm = 'O';
  driveJoint(jointA, FORWARD, get_calibration_data_arm);
  delay(200);
  runMotorForActionLength(jointA, BACKWARD);
//  delay(200);
//  driveJoint(jointB, FORWARD, GET_CALIBRATION_DATA);
//  delay(200);
//  runMotorForActionLength(jointB, BACKWARD);
//  delay(200);
//  driveJoint(jointC, FORWARD, GET_CALIBRATION_DATA);
//  delay(200);
//  runMotorForActionLength(jointC, BACKWARD);
//  delay(200);
//  driveJoint(jointD, FORWARD, GET_CALIBRATION_DATA);
//  delay(200);
//  runMotorForActionLength(jointD, BACKWARD);
//  delay(200);
//  driveJoint(claw, FORWARD, GET_CALIBRATION_DATA);
//  delay(200);
//  runMotorForActionLength(claw, BACKWARD);
}

void delayAndStop(struct joint* jointToDrive)
{
  if (!calibrating)
  {
    delayForActionLength(jointToDrive);
    jointToDrive->motor->setSpeed(0);
  }
}

void delayForActionLength(struct joint* jointToDrive)
{
  unsigned int initialTime = millis();
  unsigned int currentTime = initialTime;
  
  while (currentTime < (initialTime + ACTION_LENGTH))
  {
//    unsigned int motorValue = analogRead(jointToDrive->potPin);
//    if ((motorValue < jointToDrive->minimum) || (motorValue > jointToDrive->maximum))
//    {
//      return;
//    }
    currentTime = millis();
  }
}

void driveJoint(struct joint* jointToDrive, uint8_t dir, char actionID)
{/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  unsigned int initialValue = analogRead(jointToDrive->potPin);
  runMotorForActionLength(jointToDrive, dir);
  unsigned int finalValue = analogRead(jointToDrive->potPin);
  if (!calibrating)
  {
    sendAccordingToProtocol(actionID, finalValue - initialValue);
  }
  
  loopFlag = true;
}

void printAllPots()
{
  for (int i = 0; i < 5; i++)
  {
    readAndPrint(jointArray[i]->potPin);
  }
}

void printPotA()
{
  readAndPrint(jointA->potPin);
}

void printPotB()
{
  readAndPrint(jointB->potPin);
}

void printPotC()
{
  readAndPrint(jointC->potPin);
}

void printPotD()
{
  readAndPrint(jointD->potPin);
}

void printPotClaw()
{
  readAndPrint(claw->potPin);
}

unsigned int readAndAverage(unsigned int pot)
{
  unsigned int numberOfValuesToAvg = 5;
  unsigned int accumulatedValue = 0;
  for (int i = 0; i < numberOfValuesToAvg; i++)
  {
    accumulatedValue += analogRead(pot);
  }
  return (accumulatedValue / numberOfValuesToAvg);
}

void readAndPrint(unsigned int pot)
{
  unsigned int potValue = analogRead(pot);
  sendAccordingToProtocol('Y', potValue);
}

void removeSafetyForCalibration()
{
  calibrating = true;
}

void runMotorForActionLength(struct joint* jointToDrive, uint8_t dir)
{
  jointToDrive->motor->setSpeed(DUTY_CYCLE);
  jointToDrive->motor->run(dir);
  delayAndStop(jointToDrive);
}

void sendAccordingToProtocol(char actionID, int valueDif)
{
  Serial.write(actionID);
  for (int i = 0; i < PASS_LENGTH; i++)
  {
    Serial.write(FORMAT_FLAG_NUMBER);
  }
  printf("%d", valueDif);
  for (int i = 0; i < PASS_LENGTH; i++)
  {
    Serial.write(FORMAT_FLAG_NUMBER);
  }
}

void sendID()
{
  Serial.write("y");
}

void setSafety()
{
  calibrating = false;
}

void stopAllMotors()
{
  for (int i = 0; i < 5; i++)
  {
    jointArray[i]->motor->setSpeed(0);
  }
}

void testConnection()
{
  Serial.write("X");
}
