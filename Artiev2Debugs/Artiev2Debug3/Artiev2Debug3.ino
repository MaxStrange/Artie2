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

boolean loopFlag = true;

void setup()
{
  Serial.begin(9600);
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
  switch (inChar) {
    case DRIVE_A_CLOCKWISE:
              drive(inChar, CLOCKWISE);
              break;
    case DRIVE_A_COUNTERCLOCKWISE: 
              drive(inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_B_CLOCKWISE: 
              drive(inChar, CLOCKWISE);
              break;
    case DRIVE_B_COUNTERCLOCKWISE: 
              drive(inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_C_CLOCKWISE: 
              drive(inChar, CLOCKWISE);
              break;
    case DRIVE_C_COUNTERCLOCKWISE: 
              drive(inChar, COUNTER_CLOCKWISE);
              break;
    case DRIVE_D_CLOCKWISE:
              drive(inChar, CLOCKWISE);
              break;
    case DRIVE_D_COUNTERCLOCKWISE:
              drive(inChar, COUNTER_CLOCKWISE);
              break;
    case OPEN_GRIPPER:
              drive(inChar, CLOCKWISE);
              break;
    case CLOSE_GRIPPER:
              drive(inChar, COUNTER_CLOCKWISE);
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
  Serial.write(GET_CALIBRATION_DATA);
  Serial.write(45);
}

void drive(char actionID, boolean clockwiseOrNot)
{
  int oldValue = 200;
  driveForActionLength(clockwiseOrCounter);
  int newValue = 100;
  sendAccordingToProtocol(actionID, oldValue - newValue);
}

void driveForActionLength(boolean clockwiseOrNot)
{
  switch (clockwiseOrCounter)
  {
    case CLOCKWISE:
          delayAndStop();
          break;
    case COUNTER_CLOCKWISE:
          delayAndStop();
          break;
  }
}

void delayAndStop()
{
  delayForActionLength();
}

void delayForActionLength()
{
  unsigned int initialTime = millis();
  unsigned int currentTime = initialTime;
  
  while (currentTime < (initialTime + ACTION_LENGTH))
  {
    currentTime = millis();
  }
}

void testConnection()
{
  Serial.write("X");
}

void sendID()
{
  Serial.write("x");
}
