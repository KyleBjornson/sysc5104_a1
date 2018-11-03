/*******************************************************************
*
*  DESCRIPTION: Atomic Model ActualSpeedCalc
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

/** include files **/
#include "actualSpeedCalc.h"      // class ActualSpeedCalc
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )

#define DEBUG 0
/** public functions **/
bool braking = true;
float previousSpeed = -1;


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
********************************************************************/
Model &ActualSpeedCalc::initFunction() {
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &ActualSpeedCalc::externalFunction( const ExternalMessage &msg ) {
	if( msg.port() == brakeIntensityIn) {
		float x = float(msg.value());
		#if DEBUG
			std::cout << "Got BrakeIntensityIn " <<x <<"\n";
		#endif
		if(x > 1) x = 1;
		if(x <= 0.05) {
			if (braking || x > 0){
				#if DEBUG
					std::cout << "Forcing the system to coast.\n";
				#endif
				/* No longer braking */
				x = 0.05; //Use this to model coasting
			} else {
				x = 0;
				/* No longer braking */
				braking = false;
			}
		}
		if(x > 0){
			motorDutyCycle = 0;
			if(this->state() == passive){
				#if DEBUG
					std::cout << "System was passive.\n";
				#endif
				holdIn(active, Time::Zero );
			} else if (braking) {
				#if DEBUG
					std::cout << "System was breaking, calc the new timeout.\n";
				#endif
				/*Calculate the new timeout given that we are part way though braking already.*/
				float elapsedTime = ((msg.time().asMsecs()  - lastChange().asMsecs()) / 1000);
				/* elapsed time - old timeout will give the % remaining of slow down period */
				float timeout = elapsedTime / (MAX_BRAKING_TIMEOUT / brakeIntensity) ;
				/* multiply by the new timeout to get the new timeout remaining */
				timeout *= MAX_BRAKING_TIMEOUT/x;
				holdIn(active, Time(timeout));

			} else {
				#if DEBUG
					std::cout << "System was not breaking.\n";
				#endif
				holdIn(active, Time::Zero );
			}
			/* Set new intensity and update to show we're are now braking */
			brakeIntensity = float(msg.value());
			braking = true;
		} else {
			/* we were not braking and are not being asked to brake, resume previous operation */
			holdIn(active, nextChange());
		}
	} else if (msg.port() == motorDutyCycleIn) {
		float x = float(msg.value());
		#if DEBUG
			std::cout << "Got MotorDutyCylce(x): " <<x <<"current val:" <<motorDutyCycle<<", speed: " << speed <<"\n";
		#endif
		if(x > 100) x = 100;
		if(x > 0) {
			brakeIntensity = 0;
			if(this->state() == passive){
				#if DEBUG
					std::cout << "System was passive.\n";
				#endif
				/*We were idle handle the new speed*/
				if(speed < x){
					holdIn(active, Time::Zero );
				} else if (speed == x) {
					passivate();
				} else {
					holdIn(active, Time::Zero );
				}
			} else if (!braking) {
				#if DEBUG
					std::cout << "System was not breaking.\n";
				#endif
				/*Calculate the new timeout given that we are part way though accelerating already.*/
				float elapsedTime = ((msg.time().asMsecs()  - lastChange().asMsecs()) / 1000);
				if(x == speed) {
					#if DEBUG
						std::cout << "Passivate, the speeds are the same.\n";
					#endif
					/*current speed is new speed */
					passivate();
				} else if(speed < motorDutyCycle){
					/* We were speeding up */
					#if DEBUG
						std::cout << "We were speeding up.\n";
					#endif
					if(speed < x){
						/*We are still speeding up, proceed as we were */
						#if DEBUG
							std::cout << "We are still speeding up.\n";
						#endif
						holdIn(active, nextChange());
					} else {
						/*We are now slowing down, switch to speeding up */
						/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
						#if DEBUG
							std::cout << "We are now coasting.\n";
						#endif

						holdIn(active, Time::Zero );
					}
				} else {
					#if DEBUG
						std::cout << "We were coasting.\n";
					#endif
					/*we were coasting*/
					if(speed > x){
						#if DEBUG
							std::cout << "We are still coasting.\n";
						#endif
						/*We are still coasting, proceed as we were */
						holdIn(active, nextChange());
					} else {
						#if DEBUG
							std::cout << "We are now speeding up.\n";
						#endif
						/*We were speeding up, switch to slowing down*/
						/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
						holdIn(active, Time::Zero );
					}
				}
				/* note: (speed == motorDutyCycle) condition is never going to happen, as state would have been passive. */
			} else {
				#if DEBUG
					std::cout << "System was breaking.\n";
				#endif
				/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
				if(speed < x){
					holdIn(active, Time::Zero );
				} else if (speed == x) {
					passivate();
				} else {
					holdIn(active, Time::Zero );
				}
			}
			/* Set new intensity and update to show we're are now braking */
			#if DEBUG
				std::cout << "Update motorDutyCycle.\n";
			#endif
			motorDutyCycle = float(msg.value());
			braking = false;
		} else if (braking){
			/* we were braking and are not being asked to speed up, resume previous operation */
			holdIn(active, nextChange());
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
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
********************************************************************/
Model &ActualSpeedCalc::outputFunction( const InternalMessage &msg ){
	if (speed != previousSpeed) {
		previousSpeed = speed;
		sendOutput( msg.time(), actualSpeed, speed);
	}
	return *this ;
}
