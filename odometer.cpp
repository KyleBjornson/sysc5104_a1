/*******************************************************************
*
*  DESCRIPTION: Atomic Model Sender
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

/** include files **/
#include "gpsQueue.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: GpsQueue
* Description: 
********************************************************************/
GpsQueue::GpsQueue( const string &name )
: Atomic( name )
, speedIn( addInputPort( "speedIn" ) )
, distanceTraveled( addOutputPort( "distanceTraveled" ) )
{
	secondsToTravelOneMeter = 0;
	distance = 0;
	partOfMeterLeft = 0;
}

/*******************************************************************
* Function Name: initFunction
* Description: Resetea la lista
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &GpsQueue::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &GpsQueue::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == speedIn) {
		float x = float(msg.value());
		if (this->state() == passive) {
			if(x == 0){
				passivate();
			} else if(partOfMeterLeft == 0) {
				holdIn(active, 1/x);
				partOfMeterLeft = 1;
			} else {
				holdIn(active, partOfMeterLeft/x);
			}
		} else {
			partOfMeterLeft -= (msg.time() - lastChange())/secondsToTravelOneMeter;
			if(x == 0) passivate(); /*we are not moving, wait until we are moving again*/
			else holdIn(active, partOfMeterLeft/x);
		}
		if(x!= 0) secondsToTravelOneMeter = 1/x;
		else secondsToTravelOneMeter = 0;
	} 

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &GpsQueue::internalFunction( const InternalMessage & ){
	distance++;
	partOfMeterLeft = 1;
	holdIn(active, secondsToTravelOneMeter);
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &GpsQueue::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), distanceTraveled, distance);
	return *this ;
}
