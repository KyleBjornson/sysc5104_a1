/*******************************************************************
*
*  DESCRIPTION: Atomic Model desiredSpeedCalculator
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

#ifndef __DESIRED_SPEED_CALC_H
#define __DESIRED_SPEED_CALC_H

#include "atomic.h"     // class Atomic
#define YIELD_SPEED 5
#define STOP_DISTANCE 10
#define YIELD_DISTANCE 20
#define ACCELERATE_DISTANCE 10
#define WAIT_TIMEOUT 3

enum SignType {NONE=0, YIELD=1, STOP=2, SPEED=3};
struct Sign {
	SignType type;
	int value;
	int distance;
};
struct SpeedInstruction {
	int value;
	int distance;
};

class DesiredSpeedCalculator : public Atomic
{
public:
	DesiredSpeedCalculator( const string &name = "DesiredSpeedCalculator" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &leftRangeIn;
	const Port &centerRangeIn;
	const Port &rightRangeIn;
	const Port &speedIn;
	const Port &odometerIn;
	const Port &infrastructureIn;
	const Port &desiredSpeedReachedIn;
	Port &desiredSpeedOut;
	int leftRange;
	int centerRange;
	int rightRange;
	int speedLimit;
	int odometer;
	int speed;
	SpeedInstruction desiredSpeed;
	SpeedInstruction emergencySpeed;
	Sign nextSign;
};	// class DesiredSpeedCalculator

// ** inline ** // 
inline
string DesiredSpeedCalculator::className() const
{
	return "__DESIRED_SPEED_CALC_H" ;
}

#endif   //__DESIRED_SPEED_CALC_H
