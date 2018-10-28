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
#include "desiredSpeedCalculator.h"      // class ActualSpeedCalc
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
	leftRange = 0;
	centerRange = 0;
	rightRange = 0;
	speedLimit = 0;
	odometer = 0;
	emergencySpeed = {0, 0};
	desiredSpeed = {0, 0};
	speed = 0;
	nextSign = {NONE, 0, 0};
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
	if( msg.port() == leftRangeIn) {
		leftRange = float(msg.value());
		if (this->state() == passive) {
			passive();
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if( msg.port() == centerRangeIn) {
		centerRange = float(msg.value());
		//float brakingDistance = ((speed < desiredSpeed.value) ? (desiredSpeed.value) : (speed))^2/200;
		float brakingDistance = (speed)^2/200;
		if(CurrentCenterRange <= 2) {
			emergancySpeed = {0,1};
			holdIn(active, Time::Zero);
		} else if (CurrentCenterRange < (brakingDistance + 2)){
			emergancySpeed = {0, CurrentCenterRange};
			holdIn(active, Time::Zero);
		} else if (emergancySpeed.value != 0 || emergancySpeed.distance != 0) {
			emergencySpeed = {0, 0};
			switch(nextSign.type) {
				case NONE: 
					desiredSpeed = {CurrentSpeedLimit, ACCELERATE_DISTANCE};
					break;
				case STOP:
					desiredSpeed = {0, (nextSign.distance - odometer)};
					break;
				case YIELD:
					desiredSpeed = {YIELD_SPEED, (nextSign.distance - odometer)};
					break;
				case SPEED:
					desiredSpeed = {nextSign.value, (nextSign.distance - odometer)};
					break;
				default:
					break;
            }
			holdIn(active, Time::Zero);
		} else if (this->state() == passive) {
			passivate();
		} else {
			/* Should only happen if stopped at an intersection, wait until timeout */
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if( msg.port() == rightRangeIn) {
		rightRange = float(msg.value());
		if (this->state() == passive) {
			passive();
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == speedIn) {
		speed = float(msg.value());
		if (this->state() == passive) {
			passivate();
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == odometerIn) {
		odometer = float(msg.value());
		if (this->state() == passive) {
			if(emergancySpeed.value == 0 && emergancySpeed.distance == 0 && odometer = nextSign.distance) {
				holdIn(active, Time::Zero);
			} else {
				passivate();
			}
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == infrastructureIn) {
		if (this->state() == passive) {
			int temp = float(msg.value());
			nextSign.type = (temp & 0xC000) >> (30); //top 2 bits are used for type
			nextSign.value = ((temp & 0x3E00) >> (25))*5; //next 5 for the value if needed (in km/5hr to preserve bits)
			nextSign.distance = ((temp& 0x01FF)); //distance in m using the remianing bits
			nextSign.distance += odometer;
			switch(nextSign.type) {
				case NONE: 
					desiredSpeed = {CurrentSpeedLimit, 10};
					break;
				case STOP:
					desiredSpeed = {0, (nextSign.distance - odometer)}
					break;
				case YIELD:
					desiredSpeed = {YIELD_SPEED, (nextSign.distance - odometer)}
					break;
				case SPEED:
					desiredSpeed = {nextSign.value, (nextSign.distance - odometer)}
					break;
				default:
					break;
            }
            holdIn(active, Time::Zero);
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == desiredSpeedReachedIn) {
		if(emergancySpeed.value != 0 || emergancySpeed.distance != 0) {
			/* IF the lidar obstical detection is controlling the car, wait until path is clear */
			passivate();
		} else {
			switch(nextSign.type) {
				case NONE: 
					/*Should never happen */
					break;
				case STOP:
					if(leftRange > STOP_DISTANCE && rightRange > STOP_DISTANCE && centerRange > STOP_DISTANCE) {
						/* The intersection is clear, continue driving */
						desiredSpeed = {CurrentSpeedLimit, ACCELERATE_DISTANCE};
						nextSign.type = NONE;
						holdIn(active, Time::Zero);
					} else {
						holdIn(active, WAIT_TIMEOUT);
					}
					break;
				case YIELD:
					if(leftRange > YIELD_DISTANCE && rightRange > YIELD_DISTANCE && centerRange > YIELD_DISTANCE) {
						/* The intersection is clear, continue driving */
						desiredSpeed = {CurrentSpeedLimit, ACCELERATE_DISTANCE};
						nextSign.type = NONE;
						holdIn(active, Time::Zero);
					} else if (desiredSpeed.speed != 0){
						desiredSpeed = {0, 1}
						holdIn(active, Time::Zero);
					} else {
						holdIn(active, WAIT_TIMEOUT);
					}
					break;
				case SPEED:						
					nextSign.type = NONE;
					passivate();
					break;
				default:
					break;
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
	if(emergancySpeed.value != 0 || emergancySpeed.distance != 0) {
		/* IF the lidar obstical detection is controlling the car, wait until path is clear */
		passivate();
	} else {
		switch(nextSign.type) {
			case NONE: 
				/*Occurs if @ a clear stop or yield sign */
				passivate();
				break;
			case STOP:
				if(leftRange > STOP_DISTANCE && rightRange > STOP_DISTANCE && centerRange > STOP_DISTANCE) {
					/* The intersection is clear, continue driving */
					desiredSpeed = {CurrentSpeedLimit, ACCELERATE_DISTANCE};
					nextSign.type = NONE;
					passivate();
				} else {
					holdIn(active, WAIT_TIMEOUT);
				}
				break;
			case YIELD:
				if(leftRange > YIELD_DISTANCE && rightRange > YIELD_DISTANCE && centerRange > YIELD_DISTANCE) {
					/* The intersection is clear, continue driving */
					desiredSpeed = {CurrentSpeedLimit, ACCELERATE_DISTANCE};
					nextSign.type = NONE;
					passivate();
				} else {
					holdIn(active, WAIT_TIMEOUT);
				}
				break;
			case SPEED:						
				nextSign.type = NONE;
				passivate();
				break;
			default:
				break;
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &ActualSpeedCalc::outputFunction( const InternalMessage &msg) {
	if(emergancySpeed.value != 0 || emergancySpeed.distance != 0) {
		sendOutput( msg.time(), desiredSpeedOut, emergencySpeed);
	} else {
		sendOutput( msg.time(), desiredSpeedOut, desiredSpeed);
	}
	return *this ;
}
