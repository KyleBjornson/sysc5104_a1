/*******************************************************************
*
*  DESCRIPTION: Atomic Model ActualSpeedCalc
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

#ifndef __ACTUAL_SPEED_CALC_H
#define __ACTUAL_SPEED_CALC_H

#include "atomic.h"     // class Atomic
#define MAX_BRAKING_TIMEOUT 100*0.036
#define MOTOR_INCREASE_TIMEOUT 0.1

class ActualSpeedCalc : public Atomic
{
public:
	ActualSpeedCalc( const string &name = "ActualSpeedCalc" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &brakeIntensityIn;
	const Port &motorDutyCycleIn;
	Port &actualSpeed;
	float brakeIntensity;
	float motorDutyCycle;
	float speed;
};	// class ActualSpeedCalc

// ** inline ** // 
inline
string ActualSpeedCalc::className() const
{
	return "ActualSpeedCalc" ;
}

#endif   //__ACTUAL_SPEED_CALC_H
