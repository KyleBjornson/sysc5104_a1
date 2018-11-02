[top]
components : Car 
components : actualSpeedCalc@ActualSpeedCalc 
components : motorPwm@Pwm
components : servoPwm@Pwm
components : odometer@Odometer
out : motorVoltage turnRequest brakeIntensity servoVoltage
in : laneDetection centerRange leftRange rightRange infrastructure gps
Link : actualSpeed@ActualSpeedCalc speedIn@Car
Link : brakeIntensity@Car brakeIntensityIn@ActualSpeedCalc
Link : motorDutyCycleOut@Car motorDutyCycleIn@ActualSpeedCalc
Link : actualSpeed@ActualSpeedCalc speedIn@Odometer 
Link : distanceTraveled@Odometer  distanceTraveledOut@Car
Link : motorDutyCycleOut@Car dutyCycleIn@motorPwm
Link : pwmStateOut@motorPwm motorVoltage
Link : servoDutyCycle@Car dutyCycleIn@servoPwm
Link : pwmStateOut@servoPwm servoVoltage
Link : turnRequest@Car turnRequest
Link : brakeIntensity@Car brakeIntensity
Link : laneDetection laneDetection@Car
Link : centerRange centerRange@Car
Link : leftRange leftRange@Car
Link : rightRange rightRange@Car
Link : infrastructure infrastructure@Car
Link : gps gps@Car

[motorPwm]
period : 100

[servoPwm]
period : 10

[Car]
components : SpeedController
components : DirectionController
out : motorDutyCycleOut turnRequest brakeIntensity servoDutyCycle
in : speedIn laneDetection centerRange leftRange rightRange infrastructure gps distanceTraveledOut
Link : leftRange@Car leftRange@SpeedController 
Link : centerRange@Car centerRange@SpeedController 
Link : rightRange@Car rightRange@SpeedController 
Link : speedIn@Car speedIn@SpeedController 
Link : distanceTraveledOut@Car distanceTraveledOut@SpeedController  
Link : infrastructure@Car infrastructure@SpeedController   
Link : brakeIntensity@SpeedController brakeIntensity@Car
Link : motorDutyCycleOut@SpeedController motorDutyCycleOut@Car
Link : laneDetection@Car laneDetection@DirectionController
Link : gps@Car gps@DirectionController
Link : speedIn@Car speedIn@DirectionController
Link : turnRequest@DirectionController turnRequest@Car
Link : servoDutyCycle@DirectionController servoDutyCycle@Car


[SpeedController]
components : desiredSpeedCalculator@DesiredSpeedCalculator
components : speedDriver@SpeedDriver
out : motorDutyCycleOut  brakeIntensity 
in : speedIn centerRange leftRange rightRange infrastructure distanceTraveledOut
Link : leftRange@SpeedController leftRangeIn@desiredSpeedCalculator  
Link : centerRange@SpeedController centerRangeIn@desiredSpeedCalculator  
Link : rightRange@SpeedController rightRangeIn@desiredSpeedCalculator  
Link : speedIn@SpeedController speedIn@desiredSpeedCalculator  
Link : distanceTraveledOut@SpeedController odometerIn@desiredSpeedCalculator  
Link : infrastructure@SpeedController infrastructureIn@desiredSpeedCalculator  
Link : desiredSpeedOut@desiredSpeedCalculator desiredSpeedIn@speedDriver  
Link : desiredSpeedReached@speedDriver desiredSpeedReachedIn@desiredSpeedCalculator
Link : motorSpeedOut@speedDriver motorDutyCycleOut@SpeedController
Link : brakeIntensityOut@speedDriver brakeIntensity@SpeedController

[DirectionController]
components : gpsQueue@GpsQueue
components : steerDriver@SteerDriver 
out :  turnRequest servoDutyCycle
in : laneDetection gps speedIn
Link : gps@DirectionController gpsInstructionIn@gpsQueue
Link : speedIn@DirectionController speedIn@gpsQueue
Link : turnRequest@gpsQueue turnRequest@DirectionController 
Link : laneDetection@DirectionController wheelDirectionIn@steerDriver
Link : speedIn@DirectionController speedIn@steerDriver
Link : servoDutyCycle@SteerDriver servoDutyCycle@DirectionController
