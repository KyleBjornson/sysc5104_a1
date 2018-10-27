/*******************************************************************
*
*  DESCRIPTION: Atomic Model Pwm
*
*  AUTHOR:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

#ifndef __PWM_H
#define __PWM_H


#include "atomic.h"     // class Atomic

class Pwm : public Atomic
{
public:
	Pwm( const string &name = "Pwm" );					//Default constructor

	virtual string className() const ;
protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &dutyCycleIn;
	Port &pwmStateOut;
	float period;
	float dutyCycle;
	float positiveTimeOut;
	float zeroTimeOut;
	bool vout;

};	// class Pwm

// ** inline ** // 
inline
string Pwm::className() const
{
	return "Pwm" ;
}

#endif   //__PWM_H
