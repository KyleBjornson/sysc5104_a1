[top]
components : speedDriver@SpeedDriver 
out : motorDutyCycle 
out : brakeIntensity
in : desiredSpeedIn 
in : currentSpeedIn 
Link : desiredSpeedIn desiredSpeedIn@SpeedDriver
Link : currentSpeedIn currentSpeedIn@SpeedDriver
Link : motorDutyCycle@SteerDriver motorDutyCycle
Link : brakeIntensity@SteerDriver brakeIntensity

[speedDriver]
period : 10