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
#include <iostream.h>
#define DEBUG 1

/** public functions **/

//GPS Input: 4 MSB bits = turn direction (only use one of the bits)
//			 28 LSB bits = distance

#define TURN_MASK(msg) ((msg >> 15) & 0x0001)
#define DISTANCE_MASK(msg) (msg & 0x7FFF)

/*******************************************************************
* Function Name: GpsQueue
* Description: 
********************************************************************/
GpsQueue::GpsQueue( const string &name )
: Atomic( name )
, gpsInstructionIn( addInputPort( "gpsInstructionIn" ) )
, speedIn( addInputPort( "speedIn" ) )
, turnRequest( addOutputPort( "turnRequest" ) )
{
	speed = 0;
	nextTurn = 0;
	distance = 0;
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
	if( msg.port() == gpsInstructionIn) {
		unsigned long temp = msg.value();
		int inDistance = DISTANCE_MASK(temp);
		int inNextTurn = TURN_MASK(temp);
		#if DEBUG
			std::cout << "Got gpsIIn " <<inNextTurn << " in "  << inDistance<<"\n";
		#endif

		if (this->state() == passive && distance == 0) {
			#if DEBUG
				std::cout << "We were passive, assign d and next turn\n";
			#endif

			distance = inDistance;
			nextTurn = inNextTurn;
			if (speed > 0) {
				float timeout = distance / speed;
				#if DEBUG
					std::cout << "New wait time: " << timeout <<"\n";
				#endif
				holdIn(active, Time(timeout));
			} else passivate(); //The car is not moving, wait

		} else {
			#if DEBUG
				std::cout << "We are busy, queue the gps event and continue waiting\n";
			#endif
			GpsInstruction inst;
			inst.distance = inDistance;
			inst.direction = inNextTurn;
			instructionQueue.push(inst);
			holdIn(active, nextChange());//(msg.time() - lastChange()));
		}
	} else if (msg.port() == speedIn) {
		/*Speed in is in km/hr, this block will store it in m/s to simplify calculations.*/	
		float x = long(msg.value())* MPS_FROM_KMPH;

		if (this->state() == passive && distance == 0) {
			this->passivate();
		} else if (speed == 0 && distance > 0) {
			float timeout = distance / x;
			#if DEBUG
				std::cout << "(s=0) We were stopped, calc new wait time: " << timeout <<"\n";
			#endif
			holdIn(active, Time(timeout));
		} else if (distance > 0){
			/*This will calculate the distance traveled at the old speed*/
			distance -= (nextChange().asMsecs()) * (1/1000) * speed; //(msg.time().asMsecs() - lastChange().asMsecs()) * (1/1000) * speed;
			if(x>0) {
				/*Update speed and calculate the new timeout*/
				float timeout = distance / x;
				#if DEBUG
					std::cout << "New wait time: " << timeout <<"\n";
				#endif
				holdIn(active, Time(timeout));
			} else passivate(); //The car is not moving, wait
		} else {
			/* distance is zero, passivate*/
			passivate();
		}
		speed = x;

		#if DEBUG
			std::cout << "Store new speed: " << x <<"\n";
		#endif
	}

	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: 
********************************************************************/
Model &GpsQueue::internalFunction( const InternalMessage & ){
	/*We just output vout, select timeout related to the current output*/
	if (instructionQueue.empty()) {
		#if DEBUG
			std::cout << "No requests waiting to be serviced, passivate\n";
		#endif
		distance = 0;
		passivate();
	} else {
		#if DEBUG
			std::cout << "Service next request\n";
		#endif
		distance = instructionQueue.front().distance;
		nextTurn = instructionQueue.front().direction;
		instructionQueue.pop();
		float timeout = (distance/speed);
		holdIn(active, timeout);
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &GpsQueue::outputFunction( const InternalMessage &msg ){
	#if DEBUG
		std::cout << "Outputting a turn request\n";
	#endif
	sendOutput( msg.time(), turnRequest, nextTurn);
	return *this ;
}
