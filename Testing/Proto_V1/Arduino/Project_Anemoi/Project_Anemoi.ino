#include "kalmanfilter.h"
#include <math.h>
#include <Servo.h>
#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include "SFE_LSM9DS0.h"
#include "dimensionals.h"
#include "quatops.h"
#include "sensorhub.h"
#include "MotorManager.h"
#include "AbstractServo.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define NUM_MOTORS 4

#define ORIENT_COMMAND    0x1u
#define ORIENT_COMMAND_X  1
#define ORIENT_COMMAND_Y  2
#define ORIENT_COMMAND_Z  3
#define ACCEL_COMMAND     0x2u
#define VELOCITY_COMMAND  0x3u
#define POSITION_COMMAND  0x4u

#define ARM_COMMAND       0x5u

#define DISARM_COMMAND    0x6u

#define SLAVE_ADDRESS 0x4

//#define VERBOSE

float relativeVectorX, relativeVectorY, relativeVectorZ;

int globalVectorX, globalVectorY, globalVectorZ;

uint16_t gyroX, gyroY, gyroZ;

uint16_t curRotX, curRotY, curRotZ;

double targetAngle;

point motorPositions[NUM_MOTORS];

float errors[NUM_MOTORS];

long loops = 0;

int command = -1;

void setup() {

  Serial.begin(9600);

  SensorHub::init();

 // initialize i2c as slave
 Wire.begin(SLAVE_ADDRESS);

  delay(1000);

  //positions in nearest cm.
  //41 cm long
  //48 cm across
//       Front          |
//   0            1     |
//                      41 cm
//   2            3     |
//        BACK          |
//                      |
//   <------48 cm------->

  motorPositions[0].x = -24;
  motorPositions[0].y = 20;
  motorPositions[0].z = 0;

  motorPositions[1].x = 24;
  motorPositions[1].y = 20;
  motorPositions[1].z = 0;

  motorPositions[2].x = -24;
  motorPositions[2].y = -20;
  motorPositions[2].z = 0;
  
  motorPositions[3].x = 24;
  motorPositions[3].y = -20;
  motorPositions[3].z = 0;

  MotorManager::init();

  //MotorManager::setBasePower(10);
  
  point o;
  o.x = 0;
  o.y = 0;
  o.z = 1;
  
  
  OrientationController::setOrientation(o);
  
   Wire.onReceive(receiveData);

}



//Receive control data from Raspberry Pi here.
void receiveData(int byteCount){
  uint8_t commandByte;
  uint8_t buffer[100];
  uint8_t i = 0u;
  point cmdPoint;
 
  
  commandByte = Wire.read();
  
/*  while(i < 3) {
    buffer[i++] = Wire.read();
  } */
  
  uint8_t x = Wire.read();
  
  uint8_t y = Wire.read();
  
  uint8_t z = Wire.read();
 
  switch(commandByte)
  {
  
    case ORIENT_COMMAND:
      
      cmdPoint.x = x;//buffer[ORIENT_COMMAND_X - 1];
      cmdPoint.y = y;//buffer[ORIENT_COMMAND_Y - 1];
      cmdPoint.z = z;//buffer[ORIENT_COMMAND_Z - 1];
      
      OrientationController::setOrientation(cmdPoint);      
      
      command = 1;
      
    break;
    
    case ARM_COMMAND:
    
      MotorManager::armAll();
    
    break;
    
    case DISARM_COMMAND:
    
      MotorManager::disarmAll();
    
    break;
    
    default:
    
      command = commandByte;
    
    break;
      
   
  }
 

}








void loop() {

  long loopStart = millis();

  SensorHub::update();

//MotorManager::setBasePower((float)((sin((float)(millis() / 1000.0f)) + 1) / 2) * 10 + 5);
  
  MotorManager::update();

  if(command != -1)
  {
    Serial.print("Got ");
    Serial.println(command);
    command = -1;
  }
  /*
  point targetPoint;
  targetPoint.x = 0;
  targetPoint.y = 0;
  targetPoint.z = 1;

  point relVect = SensorHub::globalToLocal(targetPoint);


  relativeVectorX = relVect.x;
  relativeVectorY = relVect.y;
  relativeVectorZ = relVect.z;

  targetAngle = acos(relativeVectorZ / sqrt(relativeVectorX * relativeVectorX + relativeVectorY * relativeVectorY + relativeVectorZ * relativeVectorZ));

  for(int i = 0; i < NUM_MOTORS; i++)
  {

    int16_t motorX = motorPositions[i].x;
    int16_t motorY = motorPositions[i].y;
    int16_t motorZ = motorPositions[i].z;

    float intersectionX = (relativeVectorY * (relativeVectorY * motorX - relativeVectorX * motorY))/((relativeVectorX * relativeVectorX) + (relativeVectorY * relativeVectorY));

    float distX = (intersectionX - motorX);

    float intersectionY = (relativeVectorX * (relativeVectorX * motorY - motorX * relativeVectorY))/ ((relativeVectorX * relativeVectorX) + (relativeVectorY * relativeVectorY));

    float distY = (intersectionY - motorY);

    float radius = sqrt(distX * distX + distY * distY);

    float motorAngle = acos((relativeVectorX * motorX + relativeVectorY * motorY + relativeVectorZ * motorZ)/(sqrt(relativeVectorX * relativeVectorX + relativeVectorY * relativeVectorY + relativeVectorZ * relativeVectorZ) * sqrt(motorX * motorX + motorY * motorY + motorZ * motorZ))) - (M_PI_2);

    errors[i] = targetAngle * radius * motorAngle;
  }

  long dur = millis() - loopStart;*/

#ifdef VERBOSE

  Serial.print("------Errors Calculated-----\n");

  for(int i = 0; i < NUM_MOTORS; i++)
  {

    Serial.print("Motor (");
    Serial.print(i);
    Serial.print("): ");
    Serial.print(errors[i]);
    Serial.println("\n");
    
  }*/

//       Front          |
//   0            1     |
//                      y
//   2            3     |
//        BACK          |
//                      |
//   <------x------->


  Serial.print("Front:  ");


  Serial.print(errors[0]);
  Serial.print("     ");
  Serial.print(errors[1]);

  Serial.print("   Back:  ");

  Serial.print(errors[2]);
  Serial.print("     ");
  Serial.print(errors[3]);

  Serial.print("                             ");


  
  p("\r");

  

#else
    
   // loadJunk();

#endif

  //loadJunk();
  
  
}

//Loads data other than zeros for serious benchmarking
void loadJunk()
{
//       Front          |
//   0            1     |
//                      y
//   2            3     |
//        BACK          |
//                      |
//   <------x------->
 
 /*motorPositions[0][0] = -24; //X
 motorPositions[0][1] = 20; //Y
 motorPositions[0][2] = 0; //Z

 motorPositions[1][0] = 24; 
 motorPositions[1][1] = 20;
 motorPositions[1][2] = 0;

 motorPositions[2][0] = -24; 
 motorPositions[2][1] = -20;
 motorPositions[2][2] = 0;

 motorPositions[3][0] = 24; 
 motorPositions[3][1] = -20;
 motorPositions[3][2] = 0;*/

 relativeVectorX = 20;//random(1, 200);
 relativeVectorY = 10;//random(1, 200);
 relativeVectorZ = 10;random(1, 200);

#ifdef VERBOSE

  /*Serial.print("\nRelative Vectors\n");

  Serial.println(relativeVectorX);
  Serial.println(relativeVectorY);
  Serial.println(relativeVectorZ);
*/
#endif

}


#include <stdarg.h>
void p(char *fmt, ... ){
        char buf[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(buf, 128, fmt, args);
        va_end (args);
        Serial.print(buf);
}
