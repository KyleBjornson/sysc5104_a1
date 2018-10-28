[top]
components : desiredSpeedCalculator@DesiredSpeedCalculator 
out : desiredSpeedOut 
in : leftRangeIn 
in : centerRangeIn 
in : rightRangeIn 
in : speedIn 
in : odometerIn 
in : infrastructureIn 
in : desiredSpeedReachedIn 
Link : leftRangeIn leftRangeIn@DesiredSpeedCalculator  
Link : centerRangeIn centerRangeIn@DesiredSpeedCalculator  
Link : rightRangeIn rightRangeIn@DesiredSpeedCalculator  
Link : speedIn speedIn@DesiredSpeedCalculator  
Link : odometerIn odometerIn@DesiredSpeedCalculator  
Link : infrastructureIn infrastructureIn@DesiredSpeedCalculator  
Link : brakeIntensityIn brakeIntensityIn@DesiredSpeedCalculator  
Link : desiredSpeedReachedIn desiredSpeedReachedIn@DesiredSpeedCalculator  

[desiredSpeedCalculator]
