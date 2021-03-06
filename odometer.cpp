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

/** include files **/
#include "odometer.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: Odometer
********************************************************************/
Odometer::Odometer( const string &name )
: Atomic( name )
, speedIn( addInputPort( "speedIn" ) )
, distanceTraveled( addOutputPort( "distanceTraveled" ) )
{
	secondsToTravelOneMeter = 0;
	distance = 1;
	partOfMeterLeft = 1;
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Odometer::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &Odometer::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == speedIn) {
		float x = float(msg.value())/3.6;
		if (this->state() == passive) {
			if(x == 0) passivate();
			else holdIn(active, Time( static_cast<float>(partOfMeterLeft/x)));
		} else {
			partOfMeterLeft -= ((msg.time().asMsecs()  - lastChange().asMsecs()) / 1000) / secondsToTravelOneMeter;
			if(x <= 0) {
				passivate(); /*we are not moving, wait until we are moving again*/
			} else if ( partOfMeterLeft <= 0){
				holdIn(active, Time::Zero);
			} else {
				float timeout = partOfMeterLeft/x;
				holdIn(active, Time(timeout));
			}
		}
		if(x!= 0) secondsToTravelOneMeter = 1/x;
		else secondsToTravelOneMeter = 0;
	} 

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Odometer::internalFunction( const InternalMessage & ){
	distance++;
	partOfMeterLeft = 1;
	holdIn(active, Time( static_cast<float>(secondsToTravelOneMeter)));
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Odometer::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), distanceTraveled, distance);
	return *this ;
}
