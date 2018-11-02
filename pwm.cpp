/*******************************************************************
*
*  DESCRIPTION: Atomic Model PWM
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

/** include files **/
#include "pwm.h"      // class Queue
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "strutil.h"       // str2float( ... )


/** public functions **/

/*******************************************************************
* Function Name: Pwm
********************************************************************/
Pwm::Pwm( const string &name )
: Atomic( name )
, dutyCycleIn( addInputPort( "dutyCycleIn" ) )
, pwmStateOut( addOutputPort( "pwmStateOut" ) )
{
	vout = false;
	string parameter(MainSimulator::Instance().getParameter( description(), "period" ));
	period = str2float(parameter);
	dutyCycle = 0;
	positiveTimeOut=0;
	zeroTimeOut = period;
}

/*******************************************************************
* Function Name: initFunction
********************************************************************/
Model &Pwm::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
********************************************************************/
Model &Pwm::externalFunction( const ExternalMessage &msg )
{
	if( msg.port() == dutyCycleIn) {
		/*update duty cycle and converting input from % to decimal*/
		dutyCycle = (float(msg.value()))/100;
		/*calculate the new time spent high output and low output*/
		positiveTimeOut = dutyCycle * period;
		zeroTimeOut = (1-dutyCycle) * period;
		if (this->state() == passive) {
			/*when passive start with low, after timeout we can update the output*/
			vout = true;
			holdIn(active, Time::Zero);
		} else {
			/*finish current wait then next period we will use new duty cycle*/
			holdIn(active, (msg.time() - lastChange()));
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
********************************************************************/
Model &Pwm::internalFunction( const InternalMessage & ){
	/*We just output vout, select timeout related to the current output*/
	if (vout){
		holdIn(active, Time(positiveTimeOut));
	} else {
		holdIn(active, Time(zeroTimeOut));
	}
	/*Switch vout, so we output the opposite next timeout*/
	vout = !vout;
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
********************************************************************/
Model &Pwm::outputFunction( const InternalMessage &msg ){
	sendOutput( msg.time(), pwmStateOut, vout);
	return *this ;
}
