#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

typedef enum {
  DRIVE_A_CLOCKWISE = 'Q',
  DRIVE_A_COUNTERCLOCKWISE = 'R',
  DRIVE_B_CLOCKWISE = 'S',
  DRIVE_B_COUNTERCLOCKWISE = 'T',
  DRIVE_C_CLOCKWISE = 'U',
  DRIVE_C_COUNTERCLOCKWISE = 'V',
  DRIVE_D_CLOCKWISE = 'W',
  DRIVE_D_COUNTERCLOCKWISE = 'Y',
  OPEN_GRIPPER = 'Z',
  CLOSE_GRIPPER = 'a',
  SEND_ID = 'G',
  STOP_DRIVING = 'I',
  GET_CALIBRATION_DATA = 'J',
  REMOVE_SAFETY_FOR_CALIBRATION = 'H',
  TEST_CONNECTION = 't',
} ActionChar;


const char END_OF_ACTION = 'P';

const unsigned int JOINT_A_MOTOR = 1;
const unsigned int JOINT_B_MOTOR = 2;
const unsigned int JOINT_C_MOTOR = 3;
const unsigned int JOINT_D_MOTOR = 4;
const unsigned int GRIPPER_MOTOR = 1;
const boolean CLOCKWISE = true;
const boolean COUNTER_CLOCKWISE = false;
const unsigned int DUTY_CYCLE = 100;
const unsigned int ACTION_LENGTH = 2000;

Adafruit_MotorShield armShield(0x60);
Adafruit_MotorShield gripperShield(0x61);

struct jointStruct
{
  Adafruit_DCMotor* motor;
  unsigned int sensorPin;
  int minThreshold;
  int maxThreshold;
};

struct jointStruct jointA = { armShield.getMotor(JOINT_A_MOTOR), 0, 0, 100 };
struct jointStruct jointB = { armShield.getMotor(JOINT_B_MOTOR), 1, 0, 100 };
struct jointStruct jointC = { armShield.getMotor(JOINT_C_MOTOR), 2, 0, 100 };
struct jointStruct jointD = { armShield.getMotor(JOINT_D_MOTOR), 3, 0, 100 };
struct jointStruct gripper = { gripperShield.getMotor(GRIPPER_MOTOR), 4, 0, 100 };
//TODO : must change these min and max threshold values to
//whatever they are supposed to be.

boolean calibrating = false;
boolean loopFlag = true;

void setup()
{ 
  armShield.begin();
  gripperShield.begin();
  
  stopDriving();

  Serial.begin(9600);
}

void loop()
{
  while (Serial.available())
  {
    readSerial();
  }
  //if there is no serial data available, tell the mainframe
  //that it is ready to start receiving
  if (loopFlag)
  {
    Serial.write(END_OF_ACTION);
    loopFlag = false;
  }
}

void readSerial()
{
  unsigned int inChar = Serial.read();
  switch (inChar) {
    case DRIVE_A_CLOCKWISE:
              drive(&jointA, inChar, CLOCKWISE);
              break;
    case DRIVE_A_COUNTERCLOCKWISE: 
              drive(&jointA, inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_B_CLOCKWISE: 
              drive(&jointB, inChar, CLOCKWISE);
              break;
    case DRIVE_B_COUNTERCLOCKWISE: 
              drive(&jointB, inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_C_CLOCKWISE: 
              drive(&jointC, inChar, CLOCKWISE);
              break;
    case DRIVE_C_COUNTERCLOCKWISE: 
              drive(&jointC, inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_D_CLOCKWISE:
              drive(&jointD, inChar, CLOCKWISE);
              break;
    case DRIVE_D_COUNTERCLOCKWISE:
              drive(&jointD, inChar, COUNTER_CLOCKWISE);
              break;
    case OPEN_GRIPPER:
              drive(&gripper, inChar, CLOCKWISE);
              break;
    case CLOSE_GRIPPER:
              drive(&gripper, inChar, COUNTER_CLOCKWISE);
              break;
    case REMOVE_SAFETY_FOR_CALIBRATION:
              removeSafetyForCalibration();
              break;
    case GET_CALIBRATION_DATA:
              calibrate();
              break;
    case TEST_CONNECTION: 
              testConnection();
              break;
    case SEND_ID: 
              sendID();
              break;
    case STOP_DRIVING: 
              stopDriving();
              break;
    default:  break;
  }
}

void calibrate()
{
  drive(&jointB, GET_CALIBRATION_DATA, CLOCKWISE);
}

void removeSafetyForCalibration()
{
  calibrating = true;
}

void drive(struct jointStruct* joint, char actionID, boolean clockwiseOrCounter)
{
  int oldValue = analogRead(joint->sensorPin);
  driveForActionLength(clockwiseOrCounter, joint);
  int newValue = analogRead(joint->sensorPin);
  sendAccordingToProtocol(actionID, oldValue - newValue);
}

void driveForActionLength(boolean clockwiseOrCounter, struct jointStruct* joint)
{
  switch (clockwiseOrCounter)
  {
    case CLOCKWISE:
          joint->motor->run(FORWARD);
          delayAndStop(joint);
          break;
    case COUNTER_CLOCKWISE:
          joint->motor->run(BACKWARD);
          delayAndStop(joint);
          break;
  }
}

void delayAndStop(struct jointStruct* joint)
{
  if (!calibrating)
  {
    delayForActionLength(joint);
    joint->motor->setSpeed(0);
  }
}

void delayForActionLength(struct jointStruct* joint)
{
  unsigned int initialTime = millis();
  unsigned int currentTime = initialTime;
  
  while (currentTime < (initialTime + ACTION_LENGTH))
  {
    int motorValue = analogRead(joint->sensorPin);
    if ((motorValue < joint->minThreshold) ||
        (motorValue > joint->maxThreshold))
    {
      return;
    }
    currentTime = millis();
  }
}

void sendAccordingToProtocol(char actionID, int valueDif)
{
  Serial.write(actionID);
  Serial.write(valueDif);
}

void sendID()
{
  Serial.write("x");
}

void stopDriving()
{
  jointA.motor->setSpeed(0);
  jointB.motor->setSpeed(0);
  jointC.motor->setSpeed(0);
  jointD.motor->setSpeed(0);
  calibrating = false;
}

void testConnection()
{
  Serial.write("X");
}
