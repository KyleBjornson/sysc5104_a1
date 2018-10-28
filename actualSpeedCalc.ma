[top]
components : actualSpeedCalc@ActualSpeedCalc 
out : actualSpeed 
in : brakeIntensityIn 
in : motorDutyCycleIn 
Link : brakeIntensityIn brakeIntensityIn@ActualSpeedCalc 
Link : motorDutyCycleIn motorDutyCycleIn@ActualSpeedCalc 
Link : actualSpeed@ActualSpeedCalc  actualSpeed

[actualSpeedCalc]
