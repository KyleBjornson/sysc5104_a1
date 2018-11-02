/*******************************************************************
*
*  DESCRIPTION: Atomic Model Odometer
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

#ifndef __ODOMETER_H
#define __ODOMETER_H

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

#endif   //__ODOMETER_H
