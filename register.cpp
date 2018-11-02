/*******************************************************************
*
*  DESCRIPTION: Simulator::registerNewAtomics()
*
*  AUTHORS: Ben Earle, Kyle Bjornson
*
*  EMAIL: ben.earle@cmail.carleton.ca, kyle.bjornson@cmail.carleton.ca
*
*  DATE: November 2nd, 2018
*
*******************************************************************/

#include "modeladm.h" 
#include "mainsimu.h"
#include "pwm.h"
#include "actualSpeedCalc.h"
#include "gpsQueue.h"
#include "odometer.h"
#include "steerDriver.h"
#include "speedDriver.h"
#include "desiredSpeedCalculator.h"


void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Pwm>(), "Pwm" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<ActualSpeedCalc>(), "ActualSpeedCalc" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<GpsQueue>(), "GpsQueue" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Odometer>(), "Odometer" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<SteerDriver>(), "SteerDriver" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<SpeedDriver>(), "SpeedDriver" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<DesiredSpeedCalculator>(), "DesiredSpeedCalculator" ) ;
}

