/*******************************************************************
*
*  DESCRIPTION: Atomic Model SpeedDriver
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

/** include files **/
#include "speedDriver.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include <stdlib.h>

/** public functions **/

/*******************************************************************
* Function Name: SteerDriver
* Description: 
********************************************************************/
SpeedDriver::SpeedDriver( const string &name )
: Atomic( name )
, desiredSpeedIn( addInputPort( "desiredSpeedIn" ) )
, currentSpeedIn( addInputPort( "currentSpeedIn" ) )
, motorDutyCycle( addOutputPort( "motorDutyCycle" ) )
, brakeIntensity( addOutputPort( "brakeIntensity" ) )
{
	currentSpeed = 0;
	brakeIntensityOut = 0;
	desiredSpeed = 0;
	distance = 0;
	accelerationTimeout = 0;
	accelerationInterval = 0;
}

/*******************************************************************
* Function Name: initFunction
* Description: Resetea la lista
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &SpeedDriver::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &SpeedDriver::externalFunction( const ExternalMessage &msg ) {

	if (msg.port() == desiredSpeedIn) {
		desiredSpeed = int (msg.value()); //fix these..
		distance = int (msg.value());

		accelerationTimeout = (3600*2*distance)/(desiredSpeed + currentSpeed);

		accelerationInterval = accelerationTimeout / abs(desiredSpeed - currentSpeed);

		//Bounded acceleration to 0-100km/h in 10 seconds
		if (accelerationInterval < 0.1) {
			accelerationInterval = 0.1;
		}


		//Now deal with the new desiredSpeed
		if (desiredSpeed < currentSpeed) {

			brakeIntensityOut = ((abs(currentSpeed - desiredSpeed))*(abs(currentSpeed - desiredSpeed))) / (200*distance);

			//Intensity is bounded to <= 1
			if (brakeIntensityOut > 1) {
				brakeIntensityOut = 1;
			}
			holdIn(active, Time::Zero); //Deal with this now

		} else if (desiredSpeed > currentSpeed) {
			brakeIntensityOut = 0;
			holdIn(active, Time::Zero);  //Deal with this now

		} else { //No change, do nothing..
			this->passivate();
		}

	} else if (msg.port() == currentSpeedIn) {
		currentSpeed = int (msg.value());
		holdIn(active, msg.time() - lastChange());
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &SpeedDriver::internalFunction( const InternalMessage & ){
	/*We just output vout, select timeout related to the current output*/
/*	if(wheelDirection == desiredWheelDirection) {
		passivate();
	} else {
		if(wheelDirection < desiredWheelDirection) {
			wheelDirection++;		
		} else {
			wheelDirection--;		
		}
		holdIn(active, Time( static_cast<float>(speed * SPEED_TIMEOUT_MULTIPLIER)));
	}*/
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &SpeedDriver::outputFunction( const InternalMessage &msg ){
	//sendOutput( msg.time(), servoDutyCylce, (7.5 + (0.1*wheelDirection)));
	return *this ;
}
