/**
*	OrientationController.cpp
*   The implementation of the orientation controller
*	Author: Grant Oberhauser
*/

#include "OrientationController.h"

#define MOTOR_P 100
#define MOTOR_I 0
#define MOTOR_D 1


static point globalOrientationVector;

Controller * OrientationController::motorControllers;

static int motorCount;

void OrientationController::init()
{
  
	motorControllers = new Controller[MotorManager::getMotorCount()];


	for(int i = 0; i < MotorManager::getMotorCount(); i++)
	{

		motorControllers[i].init(MOTOR_P, MOTOR_I, MOTOR_D, 100.0f, 0.0f);
    
	}

 

}

void OrientationController::update()
{

	point relativeVector = SensorHub::globalToLocal(globalOrientationVector);

	float targetAngle = acos(relativeVector.z / sqrt(relativeVector.x * relativeVector.x + relativeVector.y * relativeVector.y + relativeVector.z * relativeVector.z));

	for(int i = 0; i < MotorManager::getMotorCount(); i++)
	{
	  point motorLoc = MotorManager::getMotorLocation(i);

		int16_t motorX = motorLoc.x;
		int16_t motorY = motorLoc.y;
		int16_t motorZ = motorLoc.z;

		float intersectionX = (relativeVector.y * (relativeVector.y * motorX - relativeVector.x * motorY))/((relativeVector.x * relativeVector.x) + (relativeVector.y * relativeVector.y));

		float distX = (intersectionX - motorX);

		float intersectionY = (relativeVector.x * (relativeVector.x * motorY - motorX * relativeVector.y))/ ((relativeVector.x * relativeVector.x) + (relativeVector.y * relativeVector.y));

		float distY = (intersectionY - motorY);

		float radius = sqrt(distX * distX + distY * distY);

		float motorAngle = acos((relativeVector.x * motorX + relativeVector.y * motorY + relativeVector.z * motorZ)/(sqrt(relativeVector.x * relativeVector.x + relativeVector.y * relativeVector.y + relativeVector.z * relativeVector.z) * sqrt(motorX * motorX + motorY * motorY + motorZ * motorZ))) - (M_PI_2);

		float error = targetAngle * radius * motorAngle;

		motorControllers[i].setSetpoint(0);

		motorControllers[i].setCurrentValue(-error);

    motorControllers[i].setP(MOTOR_P);

    motorControllers[i].setI(MOTOR_I);

    motorControllers[i].setD(MOTOR_D);

		motorControllers[i].update();
   
	}

}

void OrientationController::setOrientation(point o)
{

	globalOrientationVector = o;

}

void OrientationController::setHeading(float h)
{

	//TODO: Implement this

}

float OrientationController::getMotorOutput(int motor)
{
	motorControllers[motor].getOutput();

}
