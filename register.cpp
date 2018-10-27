/*******************************************************************
*
*  DESCRIPTION: Simulator::registerNewAtomics()
*
*  AUTHOR:
*
*  STUDENT#:
*
*  EMAIL:
*
*  DATE:
*
*******************************************************************/

#include "modeladm.h" 
#include "mainsimu.h"
#include "pwm.h"        // class Sender


void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Pwm>(), "Pwm" ) ;
}

