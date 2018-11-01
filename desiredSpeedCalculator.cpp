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
#include "desiredSpeedCalculator.h"      // class DesiredSpeedCalculator
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

/** public functions **/

/*******************************************************************
* Function Name: DesiredSpeedCalculator
* Description: 
********************************************************************/
DesiredSpeedCalculator::DesiredSpeedCalculator( const string &name )
: Atomic( name )
, leftRangeIn( addInputPort( "leftRangeIn" ) )
, centerRangeIn( addInputPort( "centerRangeIn" ) )
, rightRangeIn( addInputPort( "rightRangeIn" ) )
, speedIn( addInputPort( "speedIn" ) )
, odometerIn( addInputPort( "odometerIn" ) )
, infrastructureIn( addInputPort( "infrastructureIn" ) )
, desiredSpeedReachedIn( addInputPort( "desiredSpeedReachedIn" ) )
, desiredSpeedOut( addOutputPort( "desiredSpeedOut" ) )
{
	leftRange = 0;
	centerRange = 0;
	rightRange = 0;
	speedLimit = 0;
	odometer = 0;
	emergencySpeed.value = 0;
	emergencySpeed.distance = 0;
	desiredSpeed.value = 0;
	desiredSpeed.distance = 0;
	speed = 0;
	nextSign.type = NONE;
}

/*******************************************************************
* Function Name: initFunction
* Description: Resetea la lista
* Precondition: El tiempo del proximo evento interno es Infinito
********************************************************************/
Model &DesiredSpeedCalculator::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description: 
********************************************************************/
Model &DesiredSpeedCalculator::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == leftRangeIn) {
		leftRange = float(msg.value());
		if (this->state() == passive) {
			passivate();
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if( msg.port() == centerRangeIn) {
		centerRange = float(msg.value());
		//float brakingDistance = ((speed < desiredSpeed.value) ? (desiredSpeed.value) : (speed))^2/200;
		float brakingDistance = speed*speed/200;
		if(centerRange <= 2) {
			emergencySpeed.value = 0;
			emergencySpeed.distance = 1;
			holdIn(active, Time::Zero);
		} else if (centerRange < (brakingDistance + 2)){
			emergencySpeed.value = 0;
			emergencySpeed.distance = centerRange;
			holdIn(active, Time::Zero);
		} else if (emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
			emergencySpeed.value = 0;
			emergencySpeed.distance = 0;
			switch(nextSign.type) {
				case NONE: 
					desiredSpeed.value = speedLimit;
					desiredSpeed.distance = ACCELERATE_DISTANCE;
					break;
				case STOP:
					desiredSpeed.value = 0;
					desiredSpeed.distance = (nextSign.distance - odometer);
					break;
				case YIELD:
					desiredSpeed.value = YIELD_SPEED;
					desiredSpeed.distance = (nextSign.distance - odometer);
					break;
				case SPEED:
					desiredSpeed.value = nextSign.value;
					desiredSpeed.distance = (nextSign.distance - odometer);
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
			passivate();
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
			if(emergencySpeed.value == 0 && emergencySpeed.distance == 0 && odometer == nextSign.distance) {
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
			int x = (temp & 0xC0000000) >> (30);
			nextSign.type = ((x == 0) ? NONE : ((x == 1) ? STOP : ((x == 2) ? YIELD : SPEED))); //top 2 bits are used for type
			nextSign.value = ((temp & 0x3E000000) >> (25))*5; //next 5 for the value if needed (in km/5hr to preserve bits)
			nextSign.distance = ((temp& 0x01FF)); //distance in m using the remianing bits
			nextSign.distance += odometer;
			switch(nextSign.type) {
				case NONE: 
					desiredSpeed.value = speedLimit;
					desiredSpeed.distance = ACCELERATE_DISTANCE;
					break;
				case STOP:
					desiredSpeed.value = 0;
					desiredSpeed.distance = (nextSign.distance - odometer);
					break;
				case YIELD:
					desiredSpeed.value = YIELD_SPEED;
					desiredSpeed.distance = (nextSign.distance - odometer);
					break;
				case SPEED:
					desiredSpeed.value = nextSign.value;
					desiredSpeed.distance = (nextSign.distance - odometer);
					break;
				default:
					break;
			}
            holdIn(active, Time::Zero);
		} else {
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == desiredSpeedReachedIn) {
		if(emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
			/* IF the lidar obstacle detection is controlling the car, wait until path is clear */
			passivate();
		} else {
			switch(nextSign.type) {
				case NONE: 
					/*Should never happen */
					break;
				case STOP:
					if(leftRange > STOP_DISTANCE && rightRange > STOP_DISTANCE && centerRange > STOP_DISTANCE) {
						/* The intersection is clear, continue driving */
						desiredSpeed.value = speedLimit;
						desiredSpeed.distance = ACCELERATE_DISTANCE;
						nextSign.type = NONE;
						holdIn(active, Time::Zero);
					} else {
						holdIn(active, WAIT_TIMEOUT);
					}
					break;
				case YIELD:
					if(leftRange > YIELD_DISTANCE && rightRange > YIELD_DISTANCE && centerRange > YIELD_DISTANCE) {
						/* The intersection is clear, continue driving */
						desiredSpeed.value = speedLimit;
						desiredSpeed.distance = ACCELERATE_DISTANCE;
						nextSign.type = NONE;
						holdIn(active, Time::Zero);
					} else if (desiredSpeed.value != 0){
						desiredSpeed.value = 0;
						desiredSpeed.distance = 1;
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
Model &DesiredSpeedCalculator::internalFunction( const InternalMessage & ){
	if(emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
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
					desiredSpeed.value = speedLimit;
					desiredSpeed.distance = ACCELERATE_DISTANCE;
					nextSign.type = NONE;
					passivate();
				} else {
					holdIn(active, WAIT_TIMEOUT);
				}
				break;
			case YIELD:
				if(leftRange > YIELD_DISTANCE && rightRange > YIELD_DISTANCE && centerRange > YIELD_DISTANCE) {
					/* The intersection is clear, continue driving */
					desiredSpeed.value = speedLimit;
					desiredSpeed.distance = ACCELERATE_DISTANCE;
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
Model &DesiredSpeedCalculator::outputFunction( const InternalMessage &msg) {
	int out;
	if(emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
		out = ((emergencySpeed.value & 0xF) << 24) + (emergencySpeed.distance & 0x0FFF);
		sendOutput( msg.time(), desiredSpeedOut, out);
	} else {
		out = ((desiredSpeed.value & 0xF) << 24) + (desiredSpeed.distance & 0x0FFF);
		sendOutput( msg.time(), desiredSpeedOut, out);
	}
	return *this ;
}
