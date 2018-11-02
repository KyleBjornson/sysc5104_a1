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
#define DESIRED_SPEED_MASK(msg) ((msg >> 24) & 0x000000FF)
#define DISTANCE_MASK(msg)  (msg & 0x00FFFFFF)
float prevIntensity = 0;
int prevMotor = 0;
/*******************************************************************
* Function Name: SpeedDriver
* Description: 
********************************************************************/
SpeedDriver::SpeedDriver( const string &name )
: Atomic( name )
, desiredSpeedIn( addInputPort( "desiredSpeedIn" ) )
, currentSpeedIn( addInputPort( "currentSpeedIn" ) )
, motorSpeedOut( addOutputPort( "motorSpeedOut" ) )
, brakeIntensityOut( addOutputPort( "brakeIntensityOut" ) )
{
	currentSpeed = 0;
	brakeIntensity = 0;
	desiredSpeed = 0;
	distance = 0;
	accelerationTimeout = Time::Zero;
	accelerationInterval = Time::Zero;
	motorSpeed = 0;
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
		desiredSpeed = DESIRED_SPEED_MASK(int (msg.value()));
		distance = DISTANCE_MASK(int (msg.value()));

		float timeout = (distance)/(3.6*(desiredSpeed + currentSpeed));
		float interval = timeout / abs(desiredSpeed - currentSpeed);

		//Bounded acceleration to 0-100km/h in 10 seconds
		if (interval < 0.1) {
			interval = 0.1;
		}
		accelerationTimeout = Time(timeout);
		accelerationInterval = Time(interval);

		//Now deal with the new desiredSpeed
		if (desiredSpeed < currentSpeed) {

			brakeIntensity = ((abs(currentSpeed - desiredSpeed))*(abs(currentSpeed - desiredSpeed))) / (200*distance);

			//Intensity is bounded to <= 1
			if (brakeIntensity > 1) {
				brakeIntensity = 1;
			}
			holdIn(active, Time::Zero); //Deal with this now

		} else if (desiredSpeed > currentSpeed) {
			brakeIntensity = 0;
			desiredSpeed++;
			holdIn(active, Time::Zero);  //Deal with this now

		} else { //No change, do nothing..
			this->passivate();
		}

	} else if (msg.port() == currentSpeedIn) {
		currentSpeed = int (msg.value());
		holdIn(active, nextChange());
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &SpeedDriver::internalFunction( const InternalMessage & ){

	if (desiredSpeed > motorSpeed) {
		motorSpeed++;
		holdIn(active, Time(accelerationInterval));

	} else if (motorSpeed < currentSpeed) {
		motorSpeed = 0; //Coast
		holdIn(active, Time(accelerationTimeout));

	} else {
		brakeIntensity = 0;
		this->passivate();
	}

	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &SpeedDriver::outputFunction( const InternalMessage &msg ){
	if (prevIntensity != brakeIntensity) {
		prevIntensity = brakeIntensity;
		prevMotor = -1;
		sendOutput(msg.time(), brakeIntensityOut, brakeIntensity);
	} else if(prevMotor != motorSpeed) {
		prevMotor = motorSpeed;
		sendOutput(msg.time(), motorSpeedOut, motorSpeed);
	}
	return *this ;
}
