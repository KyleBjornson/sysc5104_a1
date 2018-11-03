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
#include <math.h> //sqrt

#define DEBUG 0
#define SUPRESS_ODO 1

#define TYPE_MASK(msg) ((msg >> 14) & 0x0003)
#define VALUE_MASK(msg) ((msg >> 9) & 0x001F)
#define DISTANCE_MASK(msg) (msg & 0x1FF)
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
//The desired speed calc is not very good, we need more logic to smooth the handling of obstacles in its path.
//We ran out of time on the assignment and didn't get to play around with this too much.
//We hope to improve it in the future.
Model &DesiredSpeedCalculator::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == leftRangeIn) {
		leftRange = float(msg.value());
		if (this->state() == passive) {
			passivate();
		} else {
			holdIn(active, nextChange());
		}
	} else if( msg.port() == centerRangeIn) {
		centerRange = float(msg.value());
		#if DEBUG
			std::cout << "There is an update to the center range: " << centerRange <<"\n";
		#endif
		//float brakingDistance = ((speed < desiredSpeed.value) ? (desiredSpeed.value) : (speed))^2/200;
		float brakingDistance = speed*speed/200;
		if(centerRange <= 2) {
			#if DEBUG
				std::cout << "There is an obstacle in close range.\n";
			#endif
			emergencySpeed.value = 0;
			emergencySpeed.distance = 1;
			holdIn(active, Time::Zero);
		} else if (centerRange < (brakingDistance + 2)){
			#if DEBUG
				std::cout << "There is an obstacle within risk of collision.\n";
			#endif
			emergencySpeed.value = 0;
			emergencySpeed.distance = centerRange;
			holdIn(active, Time::Zero);
		} else if (centerRange < (brakingDistance + 10)){
			#if DEBUG
				std::cout << "There is an obstacle close to being considered a risk.\n";
			#endif
			emergencySpeed.value = sqrt(200*(centerRange - 2)) ;//-2 since that is our buffer region
			emergencySpeed.distance = 10;
			holdIn(active, Time::Zero);
		} else if (emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
			#if DEBUG
				std::cout << "There are no obstacles that are within risk of collision.\n";
				std::cout << "The return to infrastructure control: " << nextSign.type << ", " << nextSign.value << ", "<< nextSign.distance << "\n";
			#endif
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
			holdIn(active, nextChange());
		}
	} else if( msg.port() == rightRangeIn) {
		rightRange = float(msg.value());
		if (this->state() == passive) {
			passivate();
		} else {
			holdIn(active, nextChange());
		}
	} else if (msg.port() == speedIn) {
		#if DEBUG
			std::cout << "Got speedIn " << float(msg.value()) <<"\n";
		#endif
		speed = float(msg.value());
		float brakingDistance = speed*speed/200;
		if(centerRange <= 2) {
			#if DEBUG
				std::cout << "There is an obstacle in close range.\n";
			#endif
			emergencySpeed.value = 0;
			emergencySpeed.distance = 1;
			holdIn(active, Time::Zero);
		} else if (centerRange < (brakingDistance + 2)){
			#if DEBUG
				std::cout << "There is an obstacle within risk of collision.\n";
			#endif
			emergencySpeed.value = 0;
			emergencySpeed.distance = centerRange;
			holdIn(active, Time::Zero);
		} else if (centerRange < (brakingDistance + 10)){
			#if DEBUG
				std::cout << "There is an obstacle close to being considered a risk.\n";
			#endif
			emergencySpeed.value = sqrt(200*(centerRange - 2));
			emergencySpeed.distance = 10;
			holdIn(active, Time::Zero);
} else if (emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
			#if DEBUG
				std::cout << "There are no obstacles that are within risk of collision.\n";
				std::cout << "The return to infrastructure control: " << nextSign.type << ", " << nextSign.value << ", "<< nextSign.distance << "\n";
			#endif
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
		} else {
			holdIn(active, nextChange());
		}

	} else if (msg.port() == odometerIn) {
		odometer = float(msg.value());
		#if DEBUG & !SUPRESS_ODO
			std::cout << "Got odometerIn " << float(msg.value()) <<"\n";
		#endif
		if (this->state() == passive) {
			if(emergencySpeed.value == 0 && emergencySpeed.distance == 0 && odometer == nextSign.distance) {
				holdIn(active, Time::Zero);
			} else {
				passivate();
			}
		} else {
			holdIn(active, nextChange());
		}
	} else if (msg.port() == infrastructureIn) {
		unsigned long temp = msg.value();
		#if DEBUG
			std::cout << "Got infrastructureIn " << temp <<"\n";
		#endif
		if (this->state() == passive) {
			int x = TYPE_MASK(temp);
			nextSign.type = ((x == 0) ? NONE : ((x == 2) ? STOP : ((x == 1) ? YIELD : SPEED))); //top 2 bits are used for type
			nextSign.value = VALUE_MASK(temp) *5; //next 5 for the value if needed (in km/5hr to preserve bits)
			nextSign.distance = DISTANCE_MASK(temp); //distance in m using the remianing bits
			nextSign.distance += odometer;
			#if DEBUG
				std::cout << "The msg contents are: " << nextSign.type << ", " << nextSign.value << ", "<< nextSign.distance << "\n";
			#endif
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
					speedLimit = nextSign.value;
					desiredSpeed.value = nextSign.value;
					desiredSpeed.distance = (nextSign.distance - odometer);
					#if DEBUG
						std::cout << "Setting the new speed limit: " << desiredSpeed.value << "\n";
					#endif
					break;
				default:
					break;
			}
            holdIn(active, Time::Zero);
		} else {
			holdIn(active, nextChange());
		}
	} else if (msg.port() == desiredSpeedReachedIn) {
		#if DEBUG
			std::cout << "We reached the desired speed: " << speed << "\n";
		#endif
		if(emergencySpeed.value != 0 || emergencySpeed.distance != 0) {
			#if DEBUG
				std::cout << "Speed was being controlled by LiDAR, wait until the path is clear " << "\n";
			#endif
			/* IF the lidar obstacle detection is controlling the car, wait until path is clear */
			passivate();
		} else {
		#if DEBUG
			std::cout << "Speed was being controlled by infrastructure. Next Sign: "  << nextSign.type << ", " << nextSign.value << ", "<< nextSign.distance << ", speed: " << speed << "\n";
		#endif
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
				if(leftRange > STOP_DISTANCE && rightRange > STOP_DISTANCE && centerRange > STOP_DISTANCE && (nextSign.distance <= odometer) ) {
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
				if(leftRange > YIELD_DISTANCE && rightRange > YIELD_DISTANCE && centerRange > YIELD_DISTANCE && (nextSign.distance <= odometer) ) {
					/* The intersection is clear, continue driving */
					desiredSpeed.value = speedLimit;
					desiredSpeed.distance = ACCELERATE_DISTANCE;
					nextSign.type = NONE;
					holdIn(active, Time::Zero);
				} else {
					holdIn(active, WAIT_TIMEOUT);
				}
				break;
			case SPEED:
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
		#if DEBUG
			std::cout << "Sending new emergency speed " << emergencySpeed.value << "\n";
		#endif
		if(emergencySpeed.distance > 0x1FF) emergencySpeed.distance =0x1FFF;
		if(emergencySpeed.value > 0x7F) emergencySpeed.value =0x7F;
		out = ((emergencySpeed.value & 0x7F) << 9) + (emergencySpeed.distance & 0x1FF);
		sendOutput( msg.time(), desiredSpeedOut, out);
	} else {
		#if DEBUG
			std::cout << "Sending new desired speed " << desiredSpeed.value << "\n";
		#endif
		if(desiredSpeed.distance > 0x1FF) desiredSpeed.distance =0x1FFF;
		if(desiredSpeed.value > 0x7F) desiredSpeed.value =0x7F;
		out = ((desiredSpeed.value & 0x7F) << 9) + (desiredSpeed.distance & 0x1FF);
		sendOutput( msg.time(), desiredSpeedOut, out);
	}
	return *this ;
}
