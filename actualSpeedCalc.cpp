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
, motorDutyCycleIn( addInputPort( "motorDutyCycleIn" ) )
, actualSpeed( addOutputPort( "actualSpeed" ) )
{
	brakeIntensity = 0;
	motorDutyCycle = 0;
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
		int old = brakeIntensity;
		brakeIntensity = float(msg.value());
		if(brakeIntensity > 1) brakeIntensity = 1;
		if(brakeIntensity < 0) brakeIntensity = 0;
		if(brakeIntensity > 0 && old == 0){
			//Stop accelerating and start braking
			motorDutyCycle = 0;
			if(speed == 0) {
				passivate();
			} else if(old == 0 || this->state() == passive){
				float timeout = MAX_BRAKING_TIMEOUT / brakeIntensity;
				holdIn(active, Time(timeout));
				speed --;
			} else {
				//continue as you were before
				holdIn(active, nextChange());
			}
		}
	} else if (msg.port() == motorDutyCycleIn) {
		int old = motorDutyCycle;
		motorDutyCycle = float(msg.value());
		if (motorDutyCycle < 0) motorDutyCycle = 0;
		if (motorDutyCycle > 100) motorDutyCycle = 100;
		if(motorDutyCycle > 0 ) {
			brakeIntensity = 0;
			if (old == 0 || this->state() == passive) {
				if(speed < motorDutyCycle) {
					speed++;
					holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
				} else if (speed == motorDutyCycle) {
					passivate();
				} else {
					speed--;
					float timeout = MAX_BRAKING_TIMEOUT / 0.05;
					holdIn(active, Time(timeout));;
				}
			} else {
				holdIn(active, (nextChange()));
			}
		}
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &ActualSpeedCalc::internalFunction( const InternalMessage & ){
	if (brakeIntensity != 0) {
		if(speed < 1) {
			speed = 0;
			passivate();
		} else {
			speed--;
			float timeout = MAX_BRAKING_TIMEOUT / brakeIntensity;
			holdIn(active, Time(timeout));
		}
	}else if(motorDutyCycle != 0) {
		if(speed < motorDutyCycle) {
			speed++;
			holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
		} else if (speed == motorDutyCycle) {
			passivate();
		} else {
			speed--;
			float timeout = MAX_BRAKING_TIMEOUT / 0.05;
			holdIn(active, Time(timeout));;
		}
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
