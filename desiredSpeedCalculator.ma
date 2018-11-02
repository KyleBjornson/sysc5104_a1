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
Link : leftRangeIn leftRangeIn@desiredSpeedCalculator  
Link : centerRangeIn centerRangeIn@desiredSpeedCalculator  
Link : rightRangeIn rightRangeIn@desiredSpeedCalculator  
Link : speedIn speedIn@desiredSpeedCalculator  
Link : odometerIn odometerIn@desiredSpeedCalculator  
Link : infrastructureIn infrastructureIn@desiredSpeedCalculator  
Link : desiredSpeedReachedIn desiredSpeedReachedIn@desiredSpeedCalculator  
Link : desiredSpeedOut@desiredSpeedCalculator desiredSpeedOut

[desiredSpeedCalculator]
