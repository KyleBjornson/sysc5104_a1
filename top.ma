[top]
components : Car 
components : actualSpeedCalc@ActualSpeedCalc 
out : motorVoltage turnRequest brakeIntensity steerState
in : laneDetection centerRange leftRange rightRange infrastructure gps
Link : actualSpeed@ActualSpeedCalc speedIn@Car
Link : brakeIntensityOut@Car brakeIntensityIn@ActualSpeedCalc
Link : motorDutyCycleOut@Car motorDutyCycleIn@ActualSpeedCalc
Link : 

[actualSpeed]

[Car]
components: SpeedController
out : motorDutyCycleOut brakeIntensityOut  motorVoltage turnRequest brakeIntensity steerState
in : speedIn laneDetection centerRange leftRange rightRange infrastructure gps

[SpeedController]
components : desiredSpeedCalculator@DesiredSpeedCalculator
components : speedDriver@SpeedDriver
components : motorPwm@Pwm
