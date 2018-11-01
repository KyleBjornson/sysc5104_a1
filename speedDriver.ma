[top]
components : speedDriver@SpeedDriver 
out : motorSpeedOut brakeIntensityOut 
in : desiredSpeedIn currentSpeedIn 
Link : desiredSpeedIn desiredSpeedIn@speedDriver
Link : currentSpeedIn currentSpeedIn@speedDriver
Link : motorSpeedOut@speedDriver motorSpeedOut
Link : brakeIntensityOut@speedDriver brakeIntensityOut

[speedDriver]


