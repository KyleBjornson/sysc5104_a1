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

/** public functions **/

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
		//GpsInstruction x = gpsInstruction(msg.value()); /*Yikes, I sure hope I can just do this... Sorry :P*/
		int x = int (msg.value());
		distance = x & (0x7FFF);
		nextTurn = (x & (0x8000)) >> 31;
		if (this->state() == passive) {
			holdIn(active, (distance / speed));
		} else {
			GpsInstruction inst;
			inst.distance = distance;
			inst.direction = nextTurn;
			instructionQueue.push(inst);
			holdIn(active, (msg.time() - lastChange()));
		}
	} else if (msg.port() == speedIn) {
		/*Speed in is in km/hr, this block will store it in m/s to simplify calculations.*/	
		float x = float(msg.value())* MPS_FROM_KMPH; 
		if (this->state() == passive) {
			this->passivate();
		} else {
			/*This will calculate the distance traveled at the old speed*/
			distance -= (msg.time() - lastChange()) * speed;
			/*Update speed and calculate the new timeout*/
			holdIn(active, Time( static_cast<float>((distance / speed))));
		}
		speed = x;
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
		passivate();
	} else {
		distance = instructionQueue.front().distance;
		nextTurn = instructionQueue.front().direction;
		instructionQueue.pop();
		holdIn(active, (distance / speed));
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: 
********************************************************************/
Model &GpsQueue::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), turnRequest, nextTurn);
	return *this ;
}
