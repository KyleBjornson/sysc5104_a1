/*******************************************************************
*
*  DESCRIPTION: Atomic Model Speed Driver
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

#ifndef __SPEED_DRIVER_H
#define __SPEED_DRIVER_H

#include "atomic.h"     // class Atomic

//#define SPEED_TIMEOUT_MULTIPLIER 1/(15*25)
//#define MPS_FROM_KMPH 1/3.6

class SpeedDriver : public Atomic
{
public:
	SpeedDriver( const string &name = "SpeedDriver" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &desiredSpeedIn;
	const Port &currentSpeedIn;
	Port &motorDutyCycle;
	Port &brakeIntensity;

	float currentSpeed;
	float brakeIntensityOut;
	float desiredSpeed;
	float distance;
	float accelerationTimeout;
	float accelerationInterval;
};	// class SpeedDriver

// ** inline ** // 
inline
string SpeedDriver::className() const
{
	return "SpeedDriver" ;
}

#endif   //__SPEED_DRIVER_H
