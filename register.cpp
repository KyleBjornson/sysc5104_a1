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
#include "actualSpeedCalc.h"
#include "gpsQueue.h"
#include "odometer.h"
#include "steerDriver.h"
#include "desiredSpeedCalculator.h"


void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Pwm>(), "Pwm" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<ActualSpeedCalc>(), "ActualSpeedCalc" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<GpsQueue>(), "GpsQueue" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Odometer>(), "Odometer" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<SteerDriver>(), "SteerDriver" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<DesiredSpeedCalculator>(), "DesiredSpeedCalculator" ) ;
}

