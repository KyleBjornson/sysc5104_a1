[top]
components : CarController 
components : actualSpeedCalc@ActualSpeedCalc 
components : motorPwm@Pwm
components : servoPwm@Pwm
components : odometer@Odometer
out : motorVoltage turnRequest brakeIntensity servoVoltage
in : laneDetection centerRange leftRange rightRange infrastructure gps
Link : actualSpeed@ActualSpeedCalc speedIn@CarController
Link : brakeIntensity@CarController brakeIntensityIn@ActualSpeedCalc
Link : motorDutyCycleOut@CarController motorDutyCycleIn@ActualSpeedCalc
Link : actualSpeed@ActualSpeedCalc speedIn@Odometer 
Link : distanceTraveled@Odometer  distanceTraveledOut@CarController
Link : motorDutyCycleOut@CarController dutyCycleIn@motorPwm
Link : pwmStateOut@motorPwm motorVoltage
Link : servoDutyCycle@CarController dutyCycleIn@servoPwm
Link : pwmStateOut@servoPwm servoVoltage
Link : turnRequest@CarController turnRequest
Link : brakeIntensity@CarController brakeIntensity
Link : laneDetection laneDetection@CarController
Link : centerRange centerRange@CarController
Link : leftRange leftRange@CarController
Link : rightRange rightRange@CarController
Link : infrastructure infrastructure@CarController
Link : gps gps@CarController

[motorPwm]
period : 100

[servoPwm]
period : 10

[CarController]
components : SpeedController
components : DirectionController
out : motorDutyCycleOut turnRequest brakeIntensity servoDutyCycle
in : speedIn laneDetection centerRange leftRange rightRange infrastructure gps distanceTraveledOut
Link : leftRange@CarController leftRange@SpeedController 
Link : centerRange@CarController centerRange@SpeedController 
Link : rightRange@CarController rightRange@SpeedController 
Link : speedIn@CarController speedIn@SpeedController 
Link : distanceTraveledOut@CarController distanceTraveledOut@SpeedController  
Link : infrastructure@CarController infrastructure@SpeedController   
Link : brakeIntensity@SpeedController brakeIntensity@CarController
Link : motorDutyCycleOut@SpeedController motorDutyCycleOut@CarController
Link : laneDetection@CarController laneDetection@DirectionController
Link : gps@CarController gps@DirectionController
Link : speedIn@CarController speedIn@DirectionController
Link : turnRequest@DirectionController turnRequest@CarController
Link : servoDutyCycle@DirectionController servoDutyCycle@CarController


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
