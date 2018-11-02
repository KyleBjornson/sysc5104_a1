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
bool braking = true;


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
		float x = float(msg.value());
		std::cout << "Got BrakeIntensityIn " <<x <<"\n";
		if(x > 1) x = 1;
		if(x <= 0.05) {
			if (braking || x > 0){
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
				std::cout << "System was passive.\n";
				/*We were idle, start braking */
//				float timeout = MAX_BRAKING_TIMEOUT / x;
//				holdIn(active, Time(timeout));
//				if(speed > 0) speed --;

				holdIn(active, Time::Zero );
			} else if (braking) {
				std::cout << "System was breaking.\n";
				/*Calculate the new timeout given that we are part way though braking already.*/
				float elapsedTime = ((msg.time().asMsecs()  - lastChange().asMsecs()) / 1000);
				/* elapsed time - old timeout will give the % remaining of slow down period */
				float timeout = elapsedTime / (MAX_BRAKING_TIMEOUT / brakeIntensity) ;
				/* multiply by the new timeout to get the new timeout remaining */
				timeout *= MAX_BRAKING_TIMEOUT/x;
			} else {
				std::cout << "System was not breakign.\n";
				/*For simplicity we are discarding the partial km/hr that we increased, will revisit this if time permits.*/
//				float timeout = MAX_BRAKING_TIMEOUT / x;
//				holdIn(active, Time(timeout));
//				if(speed > 0) speed --;

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
		std::cout << "Got MotorDutyCylce(x): " <<x <<"current val:" <<motorDutyCycle<<", speed: " << speed <<"\n";
		if(x > 100) x = 100;
		if(x > 0) {
			brakeIntensity = 0;
			if(this->state() == passive){
				std::cout << "System was passive.\n";
				/*We were idle handle the new speed*/
				if(speed < x){
//					if(speed < 100) speed++;
//					holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
					holdIn(active, Time::Zero );
				} else if (speed == x) {
					passivate();
				} else {
					holdIn(active, Time::Zero );
//					speed--;
//					float timeout = MAX_BRAKING_TIMEOUT / 0.05; //Use this to model coasting
//					holdIn(active, Time(timeout));;
				}
			} else if (!braking) {
				std::cout << "System was not breaking.\n";
				/*Calculate the new timeout given that we are part way though accelerating already.*/
				float elapsedTime = ((msg.time().asMsecs()  - lastChange().asMsecs()) / 1000);
				if(x == speed) {
					std::cout << "Passivate, the speeds are the same.\n";
					/*current speed is new speed */
					passivate();
				} else if(speed < motorDutyCycle){
					/* We were speeding up */
					std::cout << "We were speeding up.\n";
					if(speed < x){
						/*We are still speeding up, proceed as we were */
						std::cout << "We are still speeding up.\n";
						holdIn(active, nextChange());
					} else {
						/*We are now slowing down, switch to speeding up */
						/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
						std::cout << "We are now coasting.\n";
//						speed--;
//						float timeout = MAX_BRAKING_TIMEOUT / 0.05; //Use this to model coasting
//						holdIn(active, Time(timeout));;

						holdIn(active, Time::Zero );
					}
				} else {
					std::cout << "We were coasting.\n";
					/*we were coasting*/
					if(speed > x){
						std::cout << "We are still coasting.\n";
						/*We are still coasting, proceed as we were */
						holdIn(active, nextChange());
					} else {
						std::cout << "We are now speeding up.\n";
						/*We were speeding up, switch to slowing down*/
						/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
//						if(speed < 100) speed++;
//						holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
						holdIn(active, Time::Zero );
					}
				}
				/* note: (speed == motorDutyCycle) condition is never going to happen, as state would have been passive. */
			} else {
				std::cout << "System was breaking.\n";
				/*For simplicity we are discarding the partial km/hr that we decreased, will revisit this if time permits.*/
				if(speed < x){

					holdIn(active, Time::Zero );
//					if(speed < 100) speed++;
//					holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
				} else if (speed == x) {
					passivate();
				} else {
//					speed--;
//					float timeout = MAX_BRAKING_TIMEOUT / 0.05; //Use this to model coasting
//					holdIn(active, Time(timeout));;
					holdIn(active, Time::Zero );
				}
			}
			/* Set new intensity and update to show we're are now braking */
			std::cout << "Update motorDutyCycle.\n";
			motorDutyCycle = float(msg.value());
			braking = false;
		} else if (braking){
			/* we were braking and are not being asked to speed up, resume previous operation */
			holdIn(active, nextChange());
		}
	}


//		int old = brakeIntensity;
//		brakeIntensity = float(msg.value());
//		if(brakeIntensity > 1) brakeIntensity = 1;
//		if(brakeIntensity < 0) brakeIntensity = 0;
//		if(brakeIntensity > 0 && old == 0){
//			//Stop accelerating and start braking
//			motorDutyCycle = 0;
//			if(speed == 0) {
//				passivate();
//			} else { //if(old == 0 || this->state() == passive)
//				float timeout = MAX_BRAKING_TIMEOUT / brakeIntensity;
//				holdIn(active, Time(timeout));
//				speed --;
//			}
//		} else {
//			//continue as you were before
//			holdIn(active, nextChange());
//		}
//	} else if (msg.port() == motorDutyCycleIn) {
//		int old = motorDutyCycle;
//		motorDutyCycle = float(msg.value());
//		if (motorDutyCycle < 0) motorDutyCycle = 0;
//		if (motorDutyCycle > 100) motorDutyCycle = 100;
//		if(motorDutyCycle > 0 ) {
//			brakeIntensity = 0;
//			if (old == 0 || this->state() == passive) {
//				if(speed < motorDutyCycle) {
//					speed++;
//					holdIn(active, Time( static_cast<float>(MOTOR_INCREASE_TIMEOUT)));
//				} else if (speed == motorDutyCycle) {
//					passivate();
//				} else {
//					speed--;
//					float timeout = MAX_BRAKING_TIMEOUT / 0.05;
//					holdIn(active, Time(timeout));;
//				}
//			} else {
//				holdIn(active, (nextChange()));
//			}
//		}
//	}

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
