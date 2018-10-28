/*******************************************************************
*
*  DESCRIPTION: Atomic Model Odometer
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

class Odometer : public Atomic
{
public:
	Odometer( const string &name = "Odometer" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &speedIn;
	Port &distanceTraveled;
	float secondsToTravelOneMeter;
	float distance;
	float partOfMeterLeft;
};	// class Odometer

// ** inline ** // 
inline
string Odometer::className() const
{
	return "Odometer" ;
}

#endif   //__ACTUAL_SPEED_CALC_H
