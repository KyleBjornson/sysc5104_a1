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
, brakeIntensityIn( addInputPort( "brakeIntensityIn" ) )
, motorDutyCyleIn( addInputPort( "motorDutyCyleIn" ) )
, actualSpeed( addOutputPort( "actualSpeed" ) )
{
	brakeIntensity = 0;
	motorDutyCyle = 0;
	speed = 0;
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
	if( msg.port() == brakeIntensityIn) {
		brakeIntensity = float(msg.value())/100;
		if (brakeIntensity < 0) brakeIntensity = 0;
		if (brakeIntensity > 1) brakeIntensity = 1;
		motorDutyCyle = 0;

		if (this->state() == passive) {
			holdIn(active, (MAX_BRAKING_TIMEOUT * brakeIntensity));
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == motorDutyCyleIn) {
		motorDutyCyle = float(msg.value());
		if (motorDutyCyle < 0) motorDutyCyle = 0;
		if (motorDutyCyle > 100) motorDutyCyle = 100;
		brakeIntensity = 0;
		
		if (this->state() == passive) {
			holdIn(active, MOTOR_INCREASE_TIMEOUT);
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &GpsQueue::internalFunction( const InternalMessage & ){
	if(speed < motorDutyCyle) {
		speed++;
		holdIn(active, MOTOR_INCREASE_TIMEOUT);		
	} else if (speed > motorDutyCyle) {
		speed--;		
		holdIn(active, (MAX_BRAKING_TIMEOUT * brakeIntensity));
	} else {
		passivate();
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &GpsQueue::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), turnRequest, direction);
	return *this ;
}
