[top]
components : SpeedController
components : actualSpeedCalc@ActualSpeedCalc 
components : odometer@Odometer

out : motorDutyCycleOut brakeIntensity actualSpeed
in : centerRange leftRange rightRange infrastructure
Link : actualSpeed@ActualSpeedCalc speedIn@SpeedController
Link : actualSpeed@ActualSpeedCalc actualSpeed
Link : brakeIntensity@SpeedController brakeIntensityIn@ActualSpeedCalc
Link : motorDutyCycleOut@SpeedController motorDutyCycleIn@ActualSpeedCalc

Link : actualSpeed@ActualSpeedCalc speedIn@Odometer 
Link : distanceTraveled@Odometer  distanceTraveledOut@SpeedController

Link : motorDutyCycleOut@SpeedController motorDutyCycleOut
Link : brakeIntensity@SpeedController brakeIntensity

Link : centerRange centerRange@SpeedController
Link : leftRange leftRange@SpeedController
Link : rightRange rightRange@SpeedController
Link : infrastructure infrastructure@SpeedController

[SpeedController]
components : desiredSpeedCalculator@DesiredSpeedCalculator
components : speedDriver@SpeedDriver
out : motorDutyCycleOut  brakeIntensity 
in : speedIn centerRange leftRange rightRange infrastructure distanceTraveledOut
Link : leftRange@SpeedController leftRangeIn@desiredSpeedCalculator  
Link : centerRange@SpeedController centerRangeIn@desiredSpeedCalculator  
Link : rightRange@SpeedController rightRangeIn@desiredSpeedCalculator  
Link : speedIn@SpeedController speedIn@desiredSpeedCalculator   
Link : speedIn@SpeedController currentSpeedIn@speedDriver  
Link : distanceTraveledOut@SpeedController odometerIn@desiredSpeedCalculator  
Link : infrastructure@SpeedController infrastructureIn@desiredSpeedCalculator  
Link : desiredSpeedOut@desiredSpeedCalculator desiredSpeedIn@speedDriver  
Link : desiredSpeedReached@speedDriver desiredSpeedReachedIn@desiredSpeedCalculator
Link : motorSpeedOut@speedDriver motorDutyCycleOut@SpeedController
Link : brakeIntensityOut@speedDriver brakeIntensity@SpeedController
