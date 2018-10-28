/*******************************************************************
*
*  DESCRIPTION: Atomic Model ActualSpeedCalc
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

/** include files **/
#include "actualSpeedCalc.h"      // class ActualSpeedCalc
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: ActualSpeedCalc
* Description: 
********************************************************************/
ActualSpeedCalc::ActualSpeedCalc( const string &name )
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
Model &ActualSpeedCalc::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &ActualSpeedCalc::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == brakeIntensityIn) {
		brakeIntensity = float(msg.value())/100;
		if (brakeIntensity < 0) brakeIntensity = 0;
		if (brakeIntensity > 1) brakeIntensity = 1;
		motorDutyCyle = 0;

		if (this->state() == passive) {
			float timeout = MAX_BRAKING_TIMEOUT * brakeIntensity;
			holdIn(active, Time(timeout));
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == motorDutyCyleIn) {
		motorDutyCyle = float(msg.value());
		if (motorDutyCyle < 0) motorDutyCyle = 0;
		if (motorDutyCyle > 100) motorDutyCyle = 100;
		brakeIntensity = 0;
		
		if (this->state() == passive) {
			holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
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
Model &ActualSpeedCalc::internalFunction( const InternalMessage & ){
	if(speed < motorDutyCyle) {
		speed++;
		holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));		
	} else if (speed > motorDutyCyle) {
		speed--;		
		float timeout = MAX_BRAKING_TIMEOUT * brakeIntensity;
		holdIn(active, Time(timeout));
	} else {
		passivate();
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &ActualSpeedCalc::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), actualSpeed, speed);
	return *this ;
}
