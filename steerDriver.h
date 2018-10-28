/*******************************************************************
*
*  DESCRIPTION: Atomic Model SteerDriver
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

#ifndef __STEER_DRIVER_H
#define __STEER_DRIVER_H

#include "atomic.h"     // class Atomic

#define SPEED_TIMEOUT_MULTIPLIER 1/(15*25)

class SteerDriver : public Atomic
{
public:
	SteerDriver( const string &name = "SteerDriver" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &wheelDirectionIn;
	const Port &speedIn;
	Port &servoDutyCylce;
	float speed;
	float duty;
	int wheelDirection;
	int desiredWheelDirection;
};	// class SteerDriver

// ** inline ** // 
inline
string SteerDriver::className() const
{
	return "SteerDriver" ;
}

#endif   //__STEER_DRIVER_H
