[top]
components : actualSpeedCalc@ActualSpeedCalc 
out : actualSpeed 
in : brakeIntensityIn 
in : motorDutyCyleIn 
Link : brakeIntensityIn brakeIntensityIn@ActualSpeedCalc 
Link : motorDutyCyleIn motorDutyCyleIn@ActualSpeedCalc 
Link : actualSpeed@ActualSpeedCalc  actualSpeed

[actualSpeedCalc]
