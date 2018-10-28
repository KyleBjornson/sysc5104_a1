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
#include "odometer.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: Odomoter
* Description: 
********************************************************************/
Odomoter::Odomoter( const string &name )
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
Model &Odomoter::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &Odomoter::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == speedIn) {
		float x = float(msg.value());
		if (this->state() == passive) {
			if(x == 0){
				passivate();
			} else if(partOfMeterLeft == 0) {
				holdIn(active, Time( static_cast<float>(1/x)));
				partOfMeterLeft = 1;
			} else {
				holdIn(active, Time( static_cast<float>(partOfMeterLeft/x)));
			}
		} else {
			partOfMeterLeft -= (msg.time() - lastChange())/secondsToTravelOneMeter;
			if(x == 0) passivate(); /*we are not moving, wait until we are moving again*/
			else holdIn(active, Time( static_cast<float>(partOfMeterLeft/x)));
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
Model &Odomoter::internalFunction( const InternalMessage & ){
	distance++;
	partOfMeterLeft = 1;
	holdIn(active, Time( static_cast<float>(secondsToTravelOneMeter)));
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &Odomoter::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), distanceTraveled, distance);
	return *this ;
}
