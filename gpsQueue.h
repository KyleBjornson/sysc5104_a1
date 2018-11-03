/*******************************************************************
*
*  DESCRIPTION: Atomic Model GPSQueue
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

#ifndef __GPSQUEUE_H
#define __GPSQUEUE_H

#include "atomic.h"     // class Atomic
#include <queue>        // queue

#define MPS_FROM_KMPH (1/3.6)

//LEFT=0, RIGHT=1
struct GpsInstruction {
	int direction;
	int distance;
};

class GpsQueue : public Atomic
{
public:
	GpsQueue( const string &name = "GpsQueue" );					//Default constructor
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &gpsInstructionIn;
	const Port &speedIn;
	Port &turnRequest;
	int speed;
	int distance;
	int nextTurn;
	queue<GpsInstruction> instructionQueue;
};	// class GpsQueue

// ** inline ** // 
inline
string GpsQueue::className() const
{
	return "GpsQueue" ;
}

#endif   //__GPSQUEUE_H
