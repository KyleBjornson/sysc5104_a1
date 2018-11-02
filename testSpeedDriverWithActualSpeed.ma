[top]
components : speedDriver@SpeedDriver 
components : actualSpeedCalc@ActualSpeedCalc
components : odometer@Odometer 
out : motorSpeedOut brakeIntensityOut actualSpeed distanceTraveledOut 
in : desiredSpeedIn  
Link : desiredSpeedIn desiredSpeedIn@speedDriver
Link : actualSpeed@ActualSpeedCalc currentSpeedIn@speedDriver
Link : motorSpeedOut@speedDriver motorDutyCycleIn@ActualSpeedCalc
Link : brakeIntensityOut@speedDriver brakeIntensityIn@ActualSpeedCalc
Link : actualSpeed@ActualSpeedCalc  actualSpeed
Link : motorSpeedOut@speedDriver motorSpeedOut
Link : brakeIntensityOut@speedDriver brakeIntensityOut
Link : actualSpeed@ActualSpeedCalc speedIn@Odometer 
Link : distanceTraveled@Odometer  distanceTraveledOut

[speedDriver]

[actualSpeedCalc]
