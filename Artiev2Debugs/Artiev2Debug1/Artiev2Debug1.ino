const char END_OF_ACTION = 'P';
const char SEND_ID = 'G';
const char GET_CALIBRATION_DATA = 'J';
const char TEST_CONNECTION = 't';

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
    case GET_CALIBRATION_DATA:
              calibrate();
              break;
    case TEST_CONNECTION:
              testConnection();
              break;
    case SEND_ID:
              sendID();
              break;
    default:  break;
  }
}

void calibrate()
{
  Serial.write(GET_CALIBRATION_DATA);
  Serial.write(45);
}

void testConnection()
{
  Serial.write("X");
}

void sendID()
{
  Serial.write("x");
}
